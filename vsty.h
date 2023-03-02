#pragma once

#include <limits>
#include <utility>

namespace vsty {

	//--------------------------------------------------------------------------------------------
	//type counter lifted from https://mc-deltat.github.io/articles/stateful-metaprogramming-cpp20

	template<size_t N>
	struct reader { friend auto counted_flag(reader<N>); };

	template<size_t N>
	struct setter {
		friend auto counted_flag(reader<N>) {}
		static constexpr size_t n = N;
	};

	template< auto Tag, size_t NextVal = 0 >
	[[nodiscard]] consteval auto counter_impl() {
		constexpr bool counted_past_value = requires(reader<NextVal> r) { counted_flag(r); };

		if constexpr (counted_past_value) {
			return counter_impl<Tag, NextVal + 1>();
		}
		else {
			setter<NextVal> s;
			return s.n;
		}
	}

	template< auto Tag = [] {}, auto Val = counter_impl<Tag>() >
	constexpr auto counter = Val;

	//--------------------------------------------------------------------------------------------


	/**
	* \brief General strong type
	*
	* T...the type
	* P...phantom type as unique ID
	*/
    template<typename T, auto P >
    struct strong_type_t {
        T value{};

        strong_type_t() = default;
        explicit strong_type_t(const T& v) noexcept { value = v; };
        explicit strong_type_t(T&& v) noexcept { value = std::move(v); };

        strong_type_t(const strong_type_t<T, P>& v) noexcept { value = v.value; };
        strong_type_t(strong_type_t<T, P>&& v) noexcept { value = std::move(v.value); };

        operator const T& () const noexcept { return value; }
        operator T& () noexcept { return value; }

        strong_type_t<T, P>& operator=(const T& v) noexcept { value = v; return *this; };
        strong_type_t<T, P>& operator=(T&& v) noexcept { value = std::move(v); return *this; };
        strong_type_t<T, P>& operator=(const strong_type_t<T, P>& v) noexcept { value = v.value; return *this; };
        strong_type_t<T, P>& operator=(strong_type_t<T, P>&& v) noexcept { value = std::move(v.value); return *this; };

        struct hash {
            std::size_t operator()(const strong_type_t<T, P>& tag) const noexcept { return std::hash<T>()(tag.value); };
        };
    };


	/**
	* \brief Strong type with a null value
	*
	* T...the type
	* D...default value (=null value)
	* P...phantom type as unique ID
	*/
	template<typename T, auto P, auto D>
	struct strong_type_null_t {
		T value{D};

		strong_type_null_t() = default;
		explicit strong_type_null_t(const T& v) noexcept { value = v; };
		explicit strong_type_null_t(T&& v) noexcept { value = std::move(v); };

		strong_type_null_t(const strong_type_null_t<T, P, D>& v) noexcept { value = v.value; };
		strong_type_null_t(strong_type_null_t<T, P, D>&& v) noexcept { value = std::move(v.value); };

		operator const T& () const noexcept { return value; }
		operator T& () noexcept { return value; }

		strong_type_null_t<T, P, D>& operator=(const T& v) noexcept { value = v; return *this; };
		strong_type_null_t<T, P, D>& operator=(T&& v) noexcept { value = std::move(v); return *this; };
		strong_type_null_t<T, P, D>& operator=(const strong_type_null_t<T, P, D>& v) noexcept { value = v.value; return *this; };
		strong_type_null_t<T, P, D>& operator=(strong_type_null_t<T, P, D>&& v) noexcept { value = std::move(v.value); return *this; };

		auto operator<=>(const strong_type_null_t<T, P, D>& v) noexcept { return value <=> v.value; };

		bool has_value() const noexcept { return value != D;}

		struct hash {
			std::size_t operator()(const strong_type_null_t<T, P, D>& tag) const noexcept { return std::hash<T>()(tag.value); };
		};
	};


	/**
	* \brief Strong integral type, like size_t or uint32_t. Can be split into two integral values.
	*
	* T...the integer type
	* D...default value (=null value)
	* P...phantom type as unique ID
	* U...number of upper bits (if integer is cut into 2 values), or else 0
	*/
	template<typename T, auto P, auto D = std::numeric_limits<T>::max(), size_t U = 0>
	struct strong_integral_t {
		static const size_t L = sizeof(T) * 8ul - U; //number of lower bits (if integer is cut into 2 values)

		T value{ D };

		strong_integral_t() = default;

		template<typename U>
			requires (std::is_convertible_v<std::decay_t<U>, T>&& std::is_pod_v<std::decay_t<U>>)
		explicit strong_integral_t(U&& u) noexcept : value{ static_cast<T>(u) } {};

		template<typename U>
			requires (std::is_convertible_v<std::decay_t<U>, T>&& std::is_pod_v<std::decay_t<U>>)
		void operator=(U&& rhs) noexcept { value = static_cast<T>(rhs); };

		operator const T& () const { return value; }

		operator T& () { return value; }

		auto operator<=>(const strong_integral_t<T, P, D, U>& v) noexcept { return value <=> v.value; };

		template<typename U>
			requires std::is_convertible_v<U, T>
		auto operator<(const U& v) noexcept { return value < static_cast<T>(v); };

		T operator<<(const size_t L) noexcept { return value << L; };
		T operator>>(const size_t L) noexcept { return value >> L; };
		T operator&(const size_t L) noexcept { return value & L; };

		strong_integral_t<T, P, D, U> operator++() noexcept {
			value++;
			if (!has_value()) value = 0;
			return *this;
		};

		strong_integral_t<T, P, D, U> operator++(int) noexcept {
			strong_integral_t<T, P, D, U> res = *this;
			value++;
			if (!has_value()) value = 0;
			return res;
		};

		strong_integral_t<T, P, D, U> operator--() noexcept {
			--value;
			if (!has_value()) --value;
			return *this;
		};

		strong_integral_t<T, P, D, U> operator--(int) noexcept {
			strong_integral_t<T, P, D, U> res = *this;
			value--;
			if (!has_value()) value--;
			return res;
		};

		struct hash {
			std::size_t operator()(const strong_integral_t<T, P, D, U>& tg) const { return std::hash<T>()(tg.value); };
		};

		struct equal_to {
			constexpr bool operator()(const T& lhs, const T& rhs) const { return lhs == rhs; };
		};

		bool has_value() const {
			return value != D;
		}

		void set_upper(T v) {
			const T LMASK = (1 << L) - 1;
			value = (value & LMASK) | (value << L);
		}

		T get_upper() {
			return (value >> L);
		}

		void set_lower(T v) {
			const T LMASK = (1 << L) - 1;
			const T UMASK = ((1 << U) - 1) << L;
			value = (value & UMASK) | (v & LMASK);
		}

		T get_lower() {
			const T LMASK = (1 << L) - 1;
			return (value & LMASK);
		}

	};



}



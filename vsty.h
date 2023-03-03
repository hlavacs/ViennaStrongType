#pragma once

#include <limits>
#include <utility>

namespace vsty {


	/**
	* \brief General strong type
	*
	* T...the type
	* P...phantom type as unique ID (can use __COUNTER__ or vsty::counter<>)
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

		auto operator<=>(const strong_type_t<T, P>& v) noexcept { return value <=> v.value; };

		struct equal_to {
			constexpr bool operator()(const T& lhs, const T& rhs) const { return lhs == rhs; };
		};

        struct hash {
            std::size_t operator()(const strong_type_t<T, P>& tag) const noexcept { return std::hash<T>()(tag.value); };
        };
    };


	/**
	* \brief Strong type with a null value
	*
	* T...the type
	* D...default value (=null value)
	* P...phantom type as unique ID (can use __COUNTER__ or vsty::counter<>)
	*/	
	template<typename T, auto P, auto D>
	struct strong_type_null_t : strong_type_t<T, P> {
		using strong_type_t<T,P>::value;
		static const T null{D};
		strong_type_null_t() { value = D; };
		explicit strong_type_null_t(const T& v) : strong_type_t<T,P>(v) {};
		bool has_value() const noexcept { return value != D; }
	};


	/**
	* \brief Strong integral type, like size_t or uint32_t. Can be split into two integral values (upper and lower)
	*
	* T...the integer type
	* P...phantom type as unique ID (can use __COUNTER__ or vsty::counter<>)
	* U...number of upper bits (if integer is cut into 2 values), or else 0
	*/	
	template<typename T, auto P, size_t U = 0>
		requires std::is_integral_v<std::decay_t<T>>
	struct strong_integral_t : strong_type_t<T, P>  {

		static const size_t L = sizeof(T) * 8ull - U; //number of lower bits (if integer is cut into 2 values)

		static consteval T lmask() {
			if (L == 0) return 0ull;
			if (U == 0) return ~0ull >> (64 - (L + U));
			return ~0ull >> (64 - L);
		}

		static const T LMASK = lmask();
		static const T UMASK = ~LMASK;

		using strong_type_t<T, P>::value;
		strong_integral_t() noexcept = default;
		explicit strong_integral_t(const T& v) noexcept : strong_type_t<T, P>(v) {};

		T operator<<(const size_t N) noexcept { return value << N; };
		T operator>>(const size_t N) noexcept { return value >> N; };
		T operator&(const size_t N) noexcept { return value & N; };

		auto operator++() noexcept { ++value; return *this; };
		auto operator++(int) noexcept { return strong_integral_t<T, P, U>(value++); };
		auto operator--() noexcept { --value; return *this; };
		auto operator--(int) noexcept { return strong_integral_t<T, P, U>(value--); };

		auto set_upper(T v) noexcept requires std::is_unsigned_v<std::decay_t<T>> { value = (value & LMASK) | (value << L); }
		auto get_upper()    noexcept requires std::is_unsigned_v<std::decay_t<T>> { return (value >> L); }
		auto set_lower(T v) noexcept requires std::is_unsigned_v<std::decay_t<T>> { value = (value & UMASK) | (v & LMASK); }
		auto get_lower()    noexcept requires std::is_unsigned_v<std::decay_t<T>> { return (value & LMASK); }
	};


	/**
	* \brief Strong integral type with a null value. 
	*
	* T...the type
	* D...default value (=null value)
	* P...phantom type as unique ID (can use __COUNTER__ or vsty::counter<>)
	* U...number of upper bits (if integer is cut into 2 values), or else 0
	*/
	template<typename T, auto P, auto D = std::numeric_limits<T>::max(), size_t U = 0>
	struct strong_integral_null_t : strong_type_t<T, P> {
		using strong_integral_t<T, P>::value;
		static const T null{ D };
		strong_integral_null_t() { value = D; };
		explicit strong_integral_null_t(const T& v) : strong_integral_t<T, P>(v) {};
		bool has_value() const noexcept { return value != D; }
	};


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

}



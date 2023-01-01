#pragma once

#include <limits>

namespace vsty {

	/**
	* \brief General strong type
	*
	* T...the type
	* P...phantom type as unique ID
	* D...default value (=null value)
	* U...number of upper bits (if integer is cut into 2 values)
	*/
    template<typename T, size_t P = 0>
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

        auto operator<=>(const strong_int_t<T, P, D>& v) noexcept { return value <=> v.value; };

        struct hash {
            std::size_t operator()(const strong_type_t<T, P>& tag) const noexcept { return std::hash<T>()(tag.value); };
        };
    };


	/**
	* \brief Strong type for integers.
	*
	* T...the integer type
	* P...phantom type as unique ID
	* D...default value (=null value)
	* U...number of upper bits (if integer is cut into 2 values)
	*/
	template<typename T, size_t P = 0, auto D = std::numeric_limits<T>::max(), size_t U = 0>
	struct strong_int_t {
		static const T L = sizeof(T) * 8 - U; //number of lower bits (if integer is cut into 2 values)

		static const T null = static_cast<T>(D); //numm value

		T value{ null };

		strong_int_t() = default;

		/**
		* \brief Constructor.
		* \param[in] u A POD type that is used for setting the value.
		*/
		template<typename U>
			requires (std::is_convertible_v<std::decay_t<U>, T>&& std::is_pod_v<std::decay_t<U>>)
		explicit strong_int_t(U&& u) noexcept : value{ static_cast<T>(u) } {};

		/**
		* \brief Copy assignment.
		* \param[in] rhs Any POD int type.
		*/
		template<typename U>
			requires (std::is_convertible_v<std::decay_t<U>, T>&& std::is_pod_v<std::decay_t<U>>)
		void operator=(U&& rhs) noexcept { value = static_cast<T>(rhs); };

		/**
		* \brief Yield the int value.
		* \returns the int value.
		*/
		operator const T& () const { return value; }

		/**
		* \brief Yield the int value.
		* \returns the int value.
		*/
		operator T& () { return value; }

		/**
		* \brief Comparison operator.
		* \returns the default comparison.
		*/
		auto operator<=>(const strong_int_t<T, P, D>& v) noexcept { return value <=> v.value; };

		/**
		* \brief Comparison operator.
		* \returns the comparison between this value and another int.
		*/
		template<typename U>
			requires std::is_convertible_v<U, T>
		auto operator<(const U& v) noexcept { return value < static_cast<T>(v); };

		/**
		* \brief Left shift operator.
		* \param[in] L Number of bits to left shift.
		* \returns the value left shifted.
		*/
		T operator<<(const size_t L) noexcept { return value << L; };

		/**
		* \brief Right shift operator.
		* \param[in] L Number of bits to right shift.
		* \returns the value left shifted.
		*/
		T operator>>(const size_t L) noexcept { return value >> L; };

		/**
		* \brief And operator.
		* \param[in] L Number that should be anded bitwise.
		* \returns the bew value that was anded to the old value.
		*/
		T operator&(const size_t L) noexcept { return value & L; };

		/**
		* \brief Pre-increment operator.
		* \returns the value increased by 1.
		*/
		strong_int_t<T, P, D> operator++() noexcept {
			value++;
			if (!has_value()) value = 0;
			return *this;
		};

		/**
		* \brief Post-increment operator.
		* \returns the old value before increasing by 1.
		*/
		strong_int_t<T, P, D> operator++(int) noexcept {
			strong_int_t<T, P, D> res = *this;
			value++;
			if (!has_value()) value = 0;
			return res;
		};

		/**
		* \brief Pre-decrement operator.
		* \returns the value decreased by 1.
		*/
		strong_int_t<T, P, D> operator--() noexcept {
			--value;
			if (!has_value()) --value;
			return *this;
		};

		/**
		* \brief Post-decrement operator.
		* \returns the value before decreasing by 1.
		*/
		strong_int_t<T, P, D> operator--(int) noexcept {
			strong_int_t<T, P, D> res = *this;
			value--;
			if (!has_value()) value--;
			return res;
		};

		/**
		* \brief Create a hash value.
		*/
		struct hash {
			/**
			* \param[in] tg The input int value.
			* \returns the hash of the int value.
			*/
			std::size_t operator()(const strong_int_t<T, P, D>& tg) const { return std::hash<T>()(tg.value); };
		};

		/**
		* \brief Equality comparison as function.
		* \returns true if the value is not null (the default value).
		*/
		struct equal_to {
			constexpr bool operator()(const T& lhs, const T& rhs) const { return lhs == rhs; };
		};

		/**
		* \brief Determine whether the value is not null.
		* \returns true if the value is not null (the default value).
		*/
		bool has_value() const {
			return value != null;
		}

		/**
		* \brief Set the upper value (if split into two integers).
		* \param[in] v New upper value.
		*/
		void set_upper(T v) {
			const T LMASK = (1 << L) - 1;
			value = (value & LMASK) | (value << L);
		}

		/**
		* \brief Return the upper value (if split into two integers).
		* \returns the upper value.
		*/
		T get_upper() {
			return (value >> L);
		}

		/**
		* \brief Set the lower value (if split into two integers).
		* \param[in] v New lower value.
		*/
		void set_lower(T v) {
			const T LMASK = (1 << L) - 1;
			const T UMASK = ((1 << U) - 1) << L;
			value = (value & UMASK) | (v & LMASK);
		}

		/**
		* \brief Return the lower value (if split into two integers).
		* \returns the lower value.
		*/
		T get_lower() {
			const T LMASK = (1 << L) - 1;
			return (value & LMASK);
		}

	};


}



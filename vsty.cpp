
#include "vsty.h"
#include <string>

using strong_int_t		= vsty::strong_type_t< size_t, vsty::counter<> >;
using strong_uint32_t	= vsty::strong_type_t< size_t, vsty::counter<> >;

using strong_size_t		= vsty::strong_integral_t< size_t, vsty::counter<> >;
using strong_size_t2	= vsty::strong_integral_t< size_t, vsty::counter<> >;



int main() {
	static_assert(!std::is_same_v<strong_int_t, strong_size_t>, "strong_int_t and strong_size_t are the same type!");

	static_assert(!std::is_same_v<strong_size_t, strong_uint32_t>, "strong_size_t and strong_uint32_t are the same type!");

	static_assert(!std::is_same_v<strong_size_t, strong_size_t2>, "strong_size_t and strong_size_t2 are the same type!");

	static_assert(!std::is_same_v<strong_int_t, strong_uint32_t>, "strong_int_t and strong_uint32_t are the same type!");

}


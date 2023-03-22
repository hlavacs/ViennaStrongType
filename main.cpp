
#include "vsty.h"
#include <string>

using strong_int_t		= vsty::strong_type_t< size_t, vsty::counter<> >;
using strong_uint32_t	= vsty::strong_type_t< uint32_t, vsty::counter<> >;

using strong_size_t		= vsty::strong_integral_t< size_t, vsty::counter<> >;
using strong_size_t2	= vsty::strong_integral_t< size_t, vsty::counter<> >;

static_assert(!std::is_same_v<strong_int_t, strong_size_t>, "strong_int_t and strong_size_t are the same type!");
static_assert(!std::is_same_v<strong_size_t, strong_uint32_t>, "strong_size_t and strong_uint32_t are the same type!");
static_assert(!std::is_same_v<strong_size_t, strong_size_t2>, "strong_size_t and strong_size_t2 are the same type!");
static_assert(!std::is_same_v<strong_int_t, strong_uint32_t>, "strong_int_t and strong_uint32_t are the same type!");

void f(strong_int_t a) { return; }
void g(strong_size_t a) { return; }
void h(strong_size_t2 a) { return; }

int main() {

	//f(2); //error
	//g(2); //error
	//h(3); //error

	f(strong_int_t{ 2 });
	g(strong_size_t{ 2 });
	h(strong_size_t2{ 3 });
	

}


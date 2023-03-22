#include <string>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <format>

#include "vsty.h"

using strong_int_t		= vsty::strong_type_t< size_t, vsty::counter<> >;
using strong_uint32_t	= vsty::strong_type_t< uint32_t, vsty::counter<> >;

using strong_size_t		= vsty::strong_integral_t< size_t, vsty::counter<>, 16 >;
using strong_size_t2	= vsty::strong_integral_t< uint32_t, vsty::counter<>, 8, 8 >;

using strong_float_t	= vsty::strong_type_t< float, vsty::counter<> >;
using strong_double_t	= vsty::strong_type_t< double, vsty::counter<> >;
using strong_char_t		= vsty::strong_type_t< char, vsty::counter<> >;
using strong_string_t	= vsty::strong_type_t< std::string, vsty::counter<> >;

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
	
	strong_size_t2 si{0};

	auto um = strong_size_t2::UMASK;
	auto mm = strong_size_t2::MMASK;
	auto lm = strong_size_t2::LMASK;
	std::cout << std::format("{:08x}",um) << " "  << std::format("{:08x}", mm) << " " << std::format("{:08x}", lm) << "\n";

	si.set_lower(0xFDFD);
	si.set_middle(0x0F);
	si.set_upper(0xFE);

	auto gl = si.get_lower(); 
	auto gm = si.get_middle();
	auto gu = si.get_upper();

	assert(gl == 0xFDFD);
	assert(gm == 0x0F);
	assert(gu == 0xFE);

}


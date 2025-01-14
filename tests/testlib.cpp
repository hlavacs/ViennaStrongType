
#include "VSTY.h"


using strong_int_t		= vsty::strong_type_t< int, vsty::counter<> >;
using strong_size_t		= vsty::strong_type_t< size_t, vsty::counter<> >;
using strong_size2_t	= vsty::strong_type_t< size_t, vsty::counter<> >;
using strong_uint32_t	= vsty::strong_type_t< uint32_t, vsty::counter<> >;
using strong_float_t	= vsty::strong_type_t< float, vsty::counter<> >;
using strong_double_t	= vsty::strong_type_t< double, vsty::counter<> >;
using strong_char_t		= vsty::strong_type_t< char, vsty::counter<> >;
using strong_string_t	= vsty::strong_type_t< std::string, vsty::counter<> >;

//NULL
using strong_size_null_t	= vsty::strong_type_t< size_t
							, vsty::counter<>
							, std::integral_constant<size_t, std::numeric_limits<size_t>::max() >>;

using strong_type_st_t = vsty::strong_type_t< strong_size_null_t, vsty::counter<> >;

static_assert(!std::is_same_v<strong_int_t, strong_size_t>, "strong_int_t and strong_size_t are the same type!");
static_assert(!std::is_same_v<strong_size_t, strong_uint32_t>, "strong_size_t and strong_uint32_t are the same type!");
static_assert(!std::is_same_v<strong_size_t, strong_size2_t>, "strong_size_t and strong_size_t2 are the same type!");
static_assert(!std::is_same_v<strong_int_t, strong_uint32_t>, "strong_int_t and strong_uint32_t are the same type!");

void f(strong_int_t a) { return; }
void g(strong_size_t a) { return; }
void h(strong_size2_t a) { return; }



int main() {

	//f(2); //error
	//g(2); //error
	//h(3); //error

	{
		f(strong_int_t{ 2 });
		g(strong_size_t{ 2 });
		h(strong_size2_t{ 3 });
	}

	{
		strong_size_null_t sz0;
		assert(sz0.has_value() == false);
		sz0 = 0;	
		assert(sz0.has_value() == true);
		sz0.set_bits(43105542, 0, 32);
		sz0.set_bits(25125, 32, 24);
		sz0.set_bits(193, 32+24, 8);
		assert(sz0.get_bits(0, 32) == 43105542);
		assert(sz0.get_bits(32, 24) == 25125);
		assert(sz0.get_bits(32+24) == 193);
	}

	{
		strong_type_st_t sst;
		assert(sst.has_value() == false);
		strong_size_null_t sz0;
		assert(sz0.has_value() == false);
		sst = sz0;
		assert(sst.has_value() == false);
		sz0 = 0;
		assert(sz0.has_value() == true);
		sst = sz0;
		assert(sst.has_value() == true);
	}

	{
		strong_size_null_t sz1;
		assert(sz1.has_value() == false);
		sz1 = 0;	
		assert(sz1.has_value() == true);

		sz1.set_bits(43105, 0, 16);
		sz1.set_bits(255, 16, 8);
		sz1.set_bits(103, 24, 8);
		assert(sz1.get_bits(0, 16) == 43105);
		assert(sz1.get_bits(16, 8) == 255);
		assert(sz1.get_bits(24, 8) == 103);
	}

	{
		strong_size_null_t sz2{0};
		sz2.set_bits(462752, 0, 40);
		sz2.set_bits(1036, 40);
		assert( sz2.get_bits(0, 40) == 462752);
		assert( sz2.get_bits(40) == 1036);
	}

	{
		strong_size_null_t sz3{0};
		sz3.set_bits(462752, 0, 20);
		sz3.set_bits(-1036, 20, 12);
		assert( sz3.get_bits(0, 20) == 462752);
		assert( sz3.get_bits_signed(20, 12) == -1036);
	}

	{
		strong_size_t sz4{0};
		sz4.set_bits(-0xFAFA, 0, 40);
		sz4.set_bits(0xABAB, 40, 24);
		assert( sz4.get_bits_signed(0, 40) == -0xFAFA);
		assert( sz4.get_bits_signed(40, 24) == 0xABAB);
	}

	{
		strong_size_t sz5{-0xFAFA, -0xABAB, 40};
		assert( sz5.get_bits_signed(0, 40) == -0xFAFA);
		assert( sz5.get_bits_signed(40) == -0xABAB);
	}

	{
		strong_size_t sz6{32765432, 23564, 40};
		assert( sz6.get_bits_signed(0, 40) == 32765432);
		assert( sz6.get_bits_signed(40) == 23564);
	}

	{
		strong_size_t sz7{32765432, -23564, 40};
		assert( sz7.get_bits_signed(0, 40) == 32765432);
		assert( sz7.get_bits_signed(40) == -23564);
	}

	{
		strong_size_t sz8{-32765432, 23564, 40};
		assert( sz8.get_bits_signed(0, 40) == -32765432);
		assert( sz8.get_bits_signed(40) == 23564);
	}


	/*bool itc = std::is_trivially_copyable_v<strong_size_t>;
	bool imc = std::is_move_constructible_v<strong_size_t>;
	bool icc = std::is_copy_constructible_v<strong_size_t>;
	bool ica = std::is_copy_assignable_v<strong_size_t>;
	bool ima = std::is_move_assignable_v<strong_size_t>;*/

	{
		std::atomic<strong_size_t> atom;
		atom.store(strong_size_t{ 6 });
		std::atomic<strong_size_null_t> atom2;
		atom2.store(strong_size_null_t{1});
	}

	{
		strong_size_t sz9{4};
		std::cout << "Load: " << sz9.load() << std::endl;
		sz9.store( 5 );
		std::cout << "Load: " << sz9.load() << std::endl;
		size_t i = 6;
		sz9.store( i );
		std::cout << "Load: " << sz9.load() << std::endl;

		sz9.compare_exchange_weak( i, 7 );
		std::cout << "Load: " << sz9.load() << std::endl;
	}

}


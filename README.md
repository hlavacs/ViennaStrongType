# Vienna Strong Type Library

This library lets you define variables of strong types, i.e., you cannot accidentally implicit cast them into other strong types. This can happen, e.g., when confusion the order of function parameters. As an example, think of a function accepting the parameters length (in meters) and mass (in kg). Both parameters mean something completely different.

```cpp
  auto f( float length, float mass ) { ... }
```

Now what happens if you confuse the parameter order.

```cpp
  float l{10};
  float m{1000};

  ...
  f(l, m); //correct order
  ...
  f(m, l); //wrong order -> error
```

You can avoid this situation by using strong types:

```cpp
  #include "vsty.h"

  using length_t = vsty::strong_type_t< float, vsty::counter<> >;
  using mass_t = vsty::strong_type_t< float, vsty::counter<> >;

  auto f( length_t length, mass_t mass ) { ... }

  length_t l{10};
  mass_t m{1000};

  ...
  f(l, m); //correct order -> compiles
  ...
  f(m, l); //wrong order -> compile error

```

Note the use of *vsty::counter<>* as a so called phantom type. This is required to make sure that the resulting types are seen by the compiler as to be actually different! Otherwise, for the compiler, those types would be the *same*, and the whole point would be moot!

There are two possible specializations possible.
First, using *strong_type_null_t* or *strong_integral_null_t* allows to define an additional null value. This enables to specify that the type currently holds no value at all, but is null. For instance, when using integral types, you can use the maximally possible value of this type as null value. This is the default, if you do not specify a value as template parameter. You can test whether there is a value by calling *has_value()*. Use this only if you want to be efficient about memory usage, otherwise use *std::optional* instead.

The second specialization are types *strong_integral_t* and *strong_integral_null_t*. These are specially meant for integral types like *int32_t* or *size_t* or *uint64_t*. Deriving from these types allows additionally for using increase/decrease operators, bitwise shifting, and storing two separate values in different parts of the integer. The last option is only possible for unsigned integral types.

Consider for instance using *uint64_t*. You can split the 64 bits into two 32 bit unsigned integers (lower and upper part), and access them by the functions *set_lower()*, *get_lower()*, *set_upper()*, and *get_upper()*. This might come in handy if you want to use them *both* in a compare and swap (CAS) operation and making sure that the operation is lockless (max 64 bits on many architectures).

In the following example, we use an unsigned 32 bit integer and split it into two 16 bit integers, stored in the same variable.

```cpp
  //32 bits in total, 16 bits for the upper part
  using strong_size_t2 = vsty::strong_integral_t< uint32_t, vsty::counter<>, 16 >;

  strong_size_t2 si{0};

  si.set_lower(0x0F0F); //does not compile for signed integers
  si.set_upper(0x0FF0);

  auto gl = si.get_lower();
  auto gu = si.get_upper();

  assert(gl == 0x0F0F);
  assert(gu == 0x0FF0);
```

#include <type_traits>
#include <iostream>

struct SizeOf1 { SizeOf1() { std::cout << "Size:1" << std::endl; } };
struct SizeOf2 { SizeOf2() { std::cout << "Size:2" << std::endl; } };
struct SizeOf4 { SizeOf4() { std::cout << "Size:4" << std::endl; } };
struct SizeOf8 { SizeOf8() { std::cout << "Size:8" << std::endl; } };
struct UnknownSize { UnknownSize() { std::cout << "Size:Unknown" << std::endl; } };

enum class Endian
{
	Big,
	Little
};
using BigEndian = std::integral_constant<Endian, Endian::Big>;
using LittleEndian = std::integral_constant<Endian, Endian::Little>;

template <class T>
using number_type =
typename std::conditional<
	sizeof(T) == 1,
	SizeOf1,
	typename std::conditional<
	sizeof(T) == 2,
	SizeOf2,
	typename std::conditional<
	sizeof(T) == 4,
	SizeOf4,
	typename std::conditional<
	sizeof(T) == 8,
	SizeOf8,
	UnknownSize
	>::type
	>::type
	>::type
>::type;


template<typename T>
void swap_endian(T& ui, UnknownSize)
{
}


template<typename T>
void swap_endian(T& ui, SizeOf1)
{
}

template<typename T>
void swap_endian(T& ui, SizeOf8)
{
	union EightBytes
	{
		T ui;
		uint8_t arr[8];
	};

	EightBytes fb;
	fb.ui = ui;
	// swap the endian
	std::swap(fb.arr[0], fb.arr[7]);
	std::swap(fb.arr[1], fb.arr[6]);
	std::swap(fb.arr[2], fb.arr[5]);
	std::swap(fb.arr[3], fb.arr[4]);

	ui = fb.ui;
}

template<typename T>
void swap_endian(T& ui, SizeOf4)
{
	union FourBytes
	{
		T ui;
		uint8_t arr[4];
	};

	FourBytes fb;
	fb.ui = ui;
	// swap the endian
	std::swap(fb.arr[0], fb.arr[3]);
	std::swap(fb.arr[1], fb.arr[2]);

	ui = fb.ui;
}

template<typename T>
void swap_endian(T& ui, SizeOf2)
{
	union TwoBytes
	{
		T ui;
		uint8_t arr[2];
	};

	TwoBytes fb;
	fb.ui = ui;
	// swap the endian
	std::swap(fb.arr[0], fb.arr[1]);

	ui = fb.ui;
}

template<typename T>
void swap_endian_if_arithmetic(T& val, std::true_type)
{
	swap_endian(val, number_type<T>());
}

template<typename T>
void swap_endian_if_arithmetic(T& val, std::false_type)
{
	// T is not arithmetic so do nothing
}

template<typename T>
void swap_endian_if_same_endian_is_false(T& val, std::false_type) // false_type means different endian, therefore must swap
{
	std::is_arithmetic<T> is_arithmetic_type;

	swap_endian_if_arithmetic(val, is_arithmetic_type);
}

template<typename T>
void swap_endian_if_same_endian_is_false(T& val, std::true_type)
{
	// same endian so do nothing.
}

struct MyType { char arr[12]; };

int main()
{
	int num = 1;
	std::cout << num << std::endl;
	swap_endian_if_same_endian_is_false(num, std::false_type());
	std::cout << num << std::endl;
	swap_endian_if_same_endian_is_false(num, std::false_type());
	std::cout << num << std::endl;

    return 0;
}


#include <iostream>
#include <string>
#include "bit_array.h"

#define BYTE_SIZE 8

constexpr unsigned to_allocate(size_t a, size_t b)
{
	if (a < b) return 1;
	else return a % b == 0 ? (a / b) : (a / b + 1);
}

constexpr unsigned req_size()
{
	return BYTE_SIZE * sizeof(byte4);
}

static inline size_t count_ones(byte4 number)
{
	// Function counts number of ones in binnar number
	int count = 0;
	while (number > 0)
	{
		number &= (number - 1);
		count++;
	}
	return count;
}

BitArray::BitArray()
{
	this->array = nullptr;
	this->ptr = reinterpret_cast<byte*>(this->array);
	this->real_size = 0;
	this->num_bits = 0;
}

BitArray::BitArray(size_t num_bits, unsigned long value)
{
	this->array = new byte4[to_allocate(num_bits, req_size())];
	this->real_size = to_allocate(num_bits, req_size()) * req_size();
	this->ptr = reinterpret_cast<byte*>(this->array);

	// 255 = 1111 1111 
	byte* placeholder = reinterpret_cast<byte*>(&value);

	for (unsigned i = 0; i < sizeof(byte4); i++)
	{
		ptr[i] = placeholder[i];
	}

	this->num_bits = num_bits;
};

BitArray::~BitArray()
{
	delete[] this->array;
};

BitArray::BitArray(const BitArray& b)
{
	if (b.size_in_bits() != 0)
	{
		this->num_bits = b.size_in_bits();
		this->array = new byte4[to_allocate(this->num_bits, req_size())];
		this->real_size = to_allocate(this->num_bits, req_size()) * req_size();
		this->ptr = reinterpret_cast<byte*>(this->array);

		for (unsigned i = 0; i < to_allocate(this->num_bits, req_size()); i++)
		{
			this->array[i] = b.array[i];
		}
	}
	else
	{
		this->num_bits = 0;
		this->ptr = nullptr;
		this->array = nullptr;
		this->real_size = 0;
	}

};

// Method swaps all field in classes
void BitArray::swap(BitArray& b)
{
	size_t tmp_s = b.num_bits;
	b.num_bits = this->num_bits;
	this->num_bits = tmp_s;

	tmp_s = b.real_size;
	b.real_size = this->real_size;
	this->real_size = tmp_s;
	
	byte4* tmp_a = std::move(b.array);
	b.array = std::move(this->array);
	this->array = std::move(tmp_a);

	byte* tmp_p = std::move(b.ptr);
	b.ptr = std::move(this->ptr);
	this->ptr = std::move(tmp_p);
};

void BitArray::resize(size_t num_bits, bool value)
{
	if (num_bits == 0) throw IndexErrorException("Number of bits must be a natural number (greater than 0)");
	if (num_bits == this->num_bits) return;
	if (this->array == nullptr)
	{
		this->num_bits = num_bits;
		this->array = new byte4[to_allocate(this->num_bits, req_size())];
		this->real_size = to_allocate(this->num_bits, req_size()) * req_size();
		this->ptr = reinterpret_cast<byte*>(this->array);
		for (unsigned i = 0; i < to_allocate(this->num_bits, req_size()); i++)
		{
			this->array[i] = value ? -1 : 0;
		}
		return;
	}

	BitArray tmp(num_bits);
	tmp = value ? tmp.set() : tmp.reset();
	
	size_t min_bits = this->num_bits > num_bits ? num_bits : this->num_bits;

	for (unsigned i = 0; i < to_allocate(min_bits, req_size()); i++)
	{
		tmp.array[i] = value
			? tmp.array[i] & this->array[i]
			: tmp.array[i] | this->array[i];
	}

	size_t bound = (to_allocate(min_bits, req_size())) * req_size();

	for (unsigned i = this->num_bits; i < bound && i < min_bits; i++)
	{
		tmp = tmp.set(i, value);
	}
	this->swap(tmp);
};

void BitArray::clear()
{
	// Clear data from class
	delete[] this->array;
	this->array = nullptr;
	this->num_bits = 0;
	this->ptr = nullptr;
	this->real_size = 0;
};

void BitArray::push_back(bool bit)
{
	if (this->array == nullptr)
	{
		this->array = new byte4[1];
		this->ptr = reinterpret_cast<byte*>(this->array);
		this->real_size = 32;
	}
	// Check free memory for new bit
	if (this->num_bits == this->real_size - 1)
	{
		size_t old_size = this->num_bits;
		this->resize(old_size * 2, false);
		this->num_bits = old_size;
		this->set(bit, this->num_bits++);
		return;
	}

	this->set(this->num_bits++, bit);

};

BitArray& BitArray::set()
{
	// fill all cells which used
	for (unsigned i = 0; i < this->num_bits / req_size(); i++)
	{
		this->array[i] = -1;
	}

	// Check part of number to know bits
	// Ex: 0[010] & 0111 - gives information about bits in the parentheses
	if (this->num_bits % req_size() != 0)
	{
		byte4 last_num = ((1 << (this->num_bits % req_size())) - 1);
		this->array[(this->num_bits / req_size())] = last_num;
	}

	return (*this);
};

BitArray& BitArray::set(size_t n, bool val)
{
	// Check bounds
	if (n >= this->num_bits || n < 0) throw IndexErrorException("Incorrect index");

	// place in integer array
	size_t in_array = n / req_size();
	// shift for bit operation
	byte shift = n % req_size();

	if (val)
	{
		// if val == 1
		// than we have to cheng some bit to one
		// for this we can shift 1 to n positions
		// and do | (or)
		this->array[in_array] |= ((byte4)1 << shift);
	}
	else
	{
		// if val == 0
		// than we have to cheng some bit to one
		// for this we can shift 1 to n positions,
		// inverse and do & (and)
		this->array[in_array] &= ~((byte4)1 << shift);
	}

	return (*this);

};

BitArray& BitArray::reset(size_t index)
{
	return this->set(index, false);
};

BitArray& BitArray::reset()
{
	for (unsigned i = 0; i < to_allocate(num_bits, req_size()); i++)
	{
		this->array[i] = 0;
	}
	return (*this);
};

bool BitArray::any() const
{
	// Do "or" for all bits for finding any ones
	byte4 res = 0;
	for (unsigned i = 0; i < this->num_bits / req_size(); i++)
	{
		res |= this->array[i];
	}

	if (this->num_bits % req_size() != 0)
	{
		// Check part of number to know bit
		// Ex: 0[010] & 0111 - gives information about bits in the parentheses
		size_t in_array = this->num_bits / req_size();
		size_t shift = this->num_bits % req_size();
		byte4 mask = ((byte4)1 << shift) - 1;
		res |= this->array[in_array] & mask;
	}

	return res > 0 ? true : false;
}

bool BitArray::none() const
{
	return !this->any();
}

size_t BitArray::count() const
{
	// Using speacial function which counts ones 
	// in passed number
	size_t all_ones = 0;
	for (int i = 0; i < this->num_bits / req_size(); i++)
	{
		all_ones += count_ones(this->array[i]);
	}

	if (this->num_bits % req_size() != 0)
	{
		// Count last part of array
		size_t in_array = this->num_bits / req_size();
		size_t shift = this->num_bits % req_size();
		byte4 mask = ((byte4)1 << shift) - 1;
		all_ones += count_ones(this->array[in_array] & mask);
	}
	return all_ones;
}

size_t BitArray::size_in_bits() const
{
	return this->num_bits;
}

size_t BitArray::size_in_bytes() const
{
	return to_allocate(this->num_bits, req_size()) * sizeof(byte4);
}

bool BitArray::empty() const
{
	return this->array == nullptr;
}

std::string BitArray::to_string() const
{
	// Write all bits like arrays of bytes
	std::string res = "";
	unsigned j = this->num_bits;
	for (unsigned i = 0; i < this->size_in_bytes() && j > 0; i++)
	{
		res += '[';

		unsigned k = 0;
		for (; j > 0 && k < BYTE_SIZE; j--, k++)
		{
			res += (*this)[this->num_bits - j] ? '1' : '0';
		}
		res += "] ";
	}

	return res;
}

// Operators part

BitArray BitArray::operator~() const
{
	// Inverse all bits in array
	BitArray* new_b = new BitArray(this->num_bits);

	for (unsigned i = 0; i < to_allocate(num_bits, req_size()); i++)
	{
		new_b->array[i] = ~this->array[i];
	}

	return (*new_b);
}

bool BitArray::operator[](const size_t index) const
{
	// Return any bit in array
	if (index >= this->num_bits || index < 0) throw IndexErrorException("Incorrect index");

	size_t in_array = index / req_size();
	byte shift = index % req_size();

	return (this->array[in_array] & ((byte4)1 << shift)) > 0 ? true : false;
};

BitArray& BitArray::operator=(const BitArray& b)
{
	if (this == &b)
	{
		return (*this);
	}

	BitArray tmp(b);
	this->swap(tmp);

	return (*this);
}

BitArray& BitArray::operator&=(const BitArray& b)
{
	// Do "and" of two arrays
	if (b.size_in_bytes() != this->size_in_bytes())
	{
		throw MemoryErrorException("Two arrays have diffent sizes");
	}

	for (unsigned i = 0; i < to_allocate(this->size_in_bits(), req_size()); i++)
	{
		this->array[i] &= b.array[i];
	}

	return (*this);
}

BitArray& BitArray::operator|=(const BitArray& b)
{
	// Do "or" of two arrays
	if (b.size_in_bytes() != this->size_in_bytes())
	{
		throw MemoryErrorException("Two arrays have diffent sizes");
	}

	for (unsigned i = 0; i < to_allocate(this->size_in_bits(), req_size()); i++)
	{
		this->array[i] |= b.array[i];
	}

	return (*this);
}

BitArray& BitArray::operator^=(const BitArray& b)
{
	// Do "xor" of two arrays
	if (b.size_in_bytes() != this->size_in_bytes())
	{
		throw MemoryErrorException("Two arrays have diffent sizes");
	}

	for (unsigned i = 0; i < to_allocate(this->size_in_bits(), req_size()); i++)
	{
		this->array[i] ^= b.array[i];
	}

	return (*this);
}

// Bit shifts
BitArray& BitArray::operator<<=(size_t n)
{
	BitArray tmp(this->num_bits);
	tmp.reset();

	for (unsigned i = n; i < num_bits; i++)
	{
		tmp.set(i - n, (*this)[i]);
	}

	this->swap(tmp);

	return (*this);
}

BitArray& BitArray::operator>>=(size_t n)
{
	BitArray tmp(this->num_bits);
	tmp.reset();

	for (unsigned i = 0; i < num_bits - n; i++)
	{
		tmp.set(i + n, (*this)[i]);
	}

	this->swap(tmp);

	return (*this);
}

BitArray BitArray::operator<<(size_t n) const
{
	BitArray tmp(this->num_bits);
	tmp.reset();

	for (unsigned i = n; i < num_bits; i++)
	{
		tmp.set(i - n, (*this)[i]);
	}

	return tmp;
}

BitArray BitArray::operator>>(size_t n) const
{
	BitArray tmp(this->num_bits);
	tmp.reset();

	for (unsigned i = 0; i < num_bits - n; i++)
	{
		tmp.set(i + n, (*this)[i]);
	}

	return tmp;
}
#include <iostream>
#include <stdint.h>
#include "bit_array.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <functional>

struct BitArrayTest : public testing::Test 
{
    BitArray* a,*b;

    void SetUp()
    {
        a = new BitArray(32);
        b = new BitArray(8);
    }

    void TearDown()
    {
        delete a;
        delete b;
    }
};

bool run_try_catch(std::function<void()> f)
{
    bool result = false;
    try
    {
        f();
    }
    catch(const std::exception& e)
    {
       result = true;
    }

    return result;
}

TEST_F(BitArrayTest, init_class_test)
{
    // Act
    size_t bits = a->size_in_bits();
    size_t bytes = a->size_in_bytes();

    // Asserts
    EXPECT_EQ(bytes, 4);
    EXPECT_EQ(bits, 32);
}

TEST_F(BitArrayTest, cp_construct_test)
{
    b->reset();
    *b = b->set(7, true);
    BitArray tmp(*b);

    //Asserts
    EXPECT_STREQ(b->to_string().c_str(), tmp.to_string().c_str());
    EXPECT_EQ(b->size_in_bits(), tmp.size_in_bits());
    EXPECT_EQ(b->size_in_bytes(), tmp.size_in_bytes());
}

TEST_F(BitArrayTest, swap_test)
{
    b->reset();
    a->set();
    BitArray tmp(*a);
    b->swap(*a);

     //Asserts
    ASSERT_STREQ(b->to_string().c_str(), tmp.to_string().c_str());
    EXPECT_EQ(b->size_in_bits(), tmp.size_in_bits());
    EXPECT_EQ(b->size_in_bytes(), tmp.size_in_bytes());
}

TEST_F(BitArrayTest, eq_op_test)
{
    b->reset();
    b->set(2, true);
    BitArray tmp(12);
    tmp = (*b);
    
    ASSERT_STREQ(b->to_string().c_str(), tmp.to_string().c_str());
    ASSERT_EQ(b->size_in_bits(), tmp.size_in_bits());
    ASSERT_EQ(b->size_in_bytes(), tmp.size_in_bytes());
}

TEST_F(BitArrayTest, resize_test)
{
    b->reset();
    b->set(7,true); // [00010001]
    b->set(3,true);

    a->set();
    a->reset(6);
    a->resize(7);

    b->resize(11,false); // [00010001] [000]

    ASSERT_EQ((*b)[7], true);
    ASSERT_EQ((*b)[8], false);
    ASSERT_EQ((*b)[10], false);
    ASSERT_EQ((*a)[6], false);
    ASSERT_EQ(a->size_in_bits(), 7);
    ASSERT_EQ(b->size_in_bits(), 11);

    a->resize(16, true);
    ASSERT_EQ((*a)[15], true);
    ASSERT_EQ((*a)[14], true);
    ASSERT_EQ((*a)[6], false);

    a->clear();

    a->resize(33, false);
    ASSERT_EQ((*a)[32], false);
    ASSERT_EQ(a->size_in_bits(), 33);
}

TEST_F(BitArrayTest, clear_test)
{
    a->clear();
    BitArray tmp(*a);
    ASSERT_EQ(a->size_in_bits(), 0);
    EXPECT_FALSE(run_try_catch( [&tmp]()->void { tmp.clear();} ));
}

TEST_F(BitArrayTest, push_test)
{
    a->reset();
    a->set(31, true);

    a->push_back(false);

    ASSERT_EQ(a->size_in_bits(), 33);
    ASSERT_EQ((*a)[32], false);
    ASSERT_EQ((*a)[31], true);
    ASSERT_EQ(a->size_in_bytes(), 8);

    a->clear();
    a->push_back(false);
    ASSERT_EQ(a->size_in_bits(), 1);
    ASSERT_EQ((*a)[0], false);
    ASSERT_EQ(a->size_in_bytes(), 4);
}

TEST_F(BitArrayTest, set_test)
{
    a->resize(33, true);
    a->set(32, false);
    a->set(0, false);
    BitArray tmp(*a);

    ASSERT_EQ((*a)[32], false);
    ASSERT_EQ((*a)[32], false);
    try
    {
        (*a)[33];
    }
    catch(const IndexErrorException& e)
    {
        ASSERT_STREQ("Incorrect index", e.error.c_str());
    }
    try
    {
        (*a)[-1];
    }
    catch(const IndexErrorException& e)
    {
        ASSERT_STREQ("Incorrect index", e.error.c_str());
    }

    a->set();
    ASSERT_EQ((*a)[32], true);
}

TEST_F(BitArrayTest, any_test)
{
    a->resize(33, true);
    a->reset();
    a->set(32, true);

    ASSERT_TRUE(a->any());
    a->reset();
    ASSERT_FALSE(a->any());
}

TEST_F(BitArrayTest, count_test)
{
    a->resize(33, true);
    a->set();
    a->set(15, false);

    ASSERT_EQ(a->count(), 32);
    a->reset();
    ASSERT_EQ(a->count(), 0);
}

TEST_F(BitArrayTest, empty_test)
{
    a->clear();

    ASSERT_TRUE(a->empty());
}

TEST_F(BitArrayTest, to_string_test)
{
    b->set(5);
    const char* str = "[00000100] ";
    ASSERT_STREQ(str, b->to_string().c_str());
}

TEST(OperatorsTest, log_and_test) // &=
{
    BitArray tmp_1(32);
    tmp_1.reset();
    BitArray tmp_2(8);
    tmp_2.reset();

    tmp_1.resize(13, false);
    tmp_2.resize(13, false);

    tmp_1.set(0, true);
    tmp_1.set(12, true);
    tmp_2.set(12, true);
    tmp_2.set(7, true);

    tmp_1 &= tmp_2;

    ASSERT_TRUE(tmp_1[12]);
    ASSERT_FALSE(tmp_1[0]);
    ASSERT_FALSE(tmp_1[7]);
    ASSERT_FALSE(tmp_1[5]);

    tmp_2.resize(15);

    ASSERT_FALSE(run_try_catch([&tmp_2,&tmp_1]()->void{tmp_1 &= tmp_2;}));
};

TEST(OperatorsTest, log_or_test) // |=
{
   BitArray tmp_1(32);
    tmp_1.reset();
    BitArray tmp_2(8);
    tmp_2.reset();

    tmp_1.resize(13, false);
    tmp_2.resize(13, false);

    tmp_1.set(0, true);
    tmp_1.set(12, false);
    tmp_2.set(12, true);
    tmp_2.set(7, true);

    tmp_1 |= tmp_2;

    ASSERT_TRUE(tmp_1[0]);
    ASSERT_TRUE(tmp_1[12]);
    ASSERT_TRUE(tmp_1[7]);
    ASSERT_FALSE(tmp_1[5]);

    tmp_2.resize(15);
    tmp_2.resize(16);

    ASSERT_FALSE(run_try_catch([&tmp_2,&tmp_1]()->void{tmp_1 |= tmp_2;}));

};

TEST(OperatorsTest, log_xor_test) // ^=
{
    BitArray tmp_1(32);
    tmp_1.reset();
    BitArray tmp_2(8);
    tmp_2.reset();

    tmp_1.resize(13, false);
    tmp_2.resize(13, false);

    tmp_1.set(0, true);
    tmp_1.set(12, true);
    tmp_2.set(12, true);
    tmp_2.set(7, true);

    tmp_1 ^= tmp_2;

    ASSERT_FALSE(tmp_1[12]);
    ASSERT_TRUE(tmp_1[0]);
    ASSERT_TRUE(tmp_1[7]);
    ASSERT_FALSE(tmp_1[5]);

    tmp_2.resize(15);
    tmp_2.resize(16);

    ASSERT_FALSE(run_try_catch([&tmp_2,&tmp_1]()->void{tmp_1 ^= tmp_2;}));
};

TEST(OperatorsTest, r_shift_test)
{
    BitArray tmp_1(16,7); // "[1110000] [00000000] "
    const char* res = "[00011100] [00000000] ";
    ASSERT_STREQ(res, (tmp_1 >> 3).to_string().c_str());
}

TEST(OperatorsTest, l_shift_test)
{
    BitArray tmp_1(16,15); // "[1111000] [00000000] "
    const char* res = "[11000000] [00000000] ";
    ASSERT_STREQ(res, (tmp_1 << 2).to_string().c_str());
}

TEST(OperatorsTest, ur_shift_test)
{
    BitArray tmp_1(16,7); // "[1110000] [00000000] "
    const char* res = "[00011100] [00000000] ";
    ASSERT_STREQ(res, (tmp_1 >>= 3).to_string().c_str());
}

TEST(OperatorsTest, ul_shift_test)
{
    BitArray tmp_1(16,15); // "[1110000] [00000000] "
    const char* res = "[10000000] [00000000] ";
    ASSERT_STREQ(res, (tmp_1 <<= 3).to_string().c_str());
}

TEST(OperatorsTest, inv_test)
{
    BitArray tmp_1(8,15); // [11110000] 
    const char* res = "[00001111] ";
    ASSERT_STREQ(res, (~tmp_1).to_string().c_str());
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
  
    return RUN_ALL_TESTS();

    return 0;
}
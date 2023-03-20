#include <CirtucalBuffer.h>
#include <CirtucalBufferExt.h>

#include "gtest/gtest.h"
#include <tuple>

// without meaning
struct HardTestObj {
    int val1;
    bool val2;

    HardTestObj()
        : val1(0)
        , val2(false)
    {}

    explicit HardTestObj(int val1_)
        : val1(val1_)
        , val2(false)
    {}

    explicit HardTestObj(int val1_, bool val2_)
        : val1(val1_)
        , val2(val2_)
    {}
};

TEST(CirtucalBufferTestSuite, InitializationTest) {
    CCirtucalBuffer<int> buff1(10);
    for (int i = 0; i < 10; i++) {
        buff1.push_back(5);
    }

    CCirtucalBuffer<int> buff2(10, 5);
    std::initializer_list<int> help_buff = {1, 2, 3, 4, 5, 4, 3, 2, 1};
    CCirtucalBuffer<int> buff3(help_buff.begin(), help_buff.end());
    CCirtucalBuffer<int> buff4(help_buff);
    CCirtucalBuffer<int> buff5 = buff4;

    ASSERT_EQ(buff1, buff2);
    ASSERT_EQ(buff3, buff4);
    ASSERT_EQ(buff4, buff5);
}

TEST(CirtucalBufferTestSuite, AssigmentTest) {
    CCirtucalBuffer<int> buff1{};
    buff1 = {1, 2, 3, 4};

    CCirtucalBuffer<int> buff2 = buff1;

    ASSERT_EQ(buff1, buff2);
}

TEST(CirtucalBufferTestSuite, SwapTest) {
    CCirtucalBuffer<int> buff1 = {1, 2, 3, 4, 5};
    CCirtucalBuffer<int> buff2 = buff1;
    CCirtucalBuffer<int> buff3 = {2, 2, 2};

    std::swap(buff2, buff3);

    ASSERT_EQ(buff1, buff3);
}

TEST(CirtucalBufferTestSuite, PushBackTest) {
    CCirtucalBuffer<int> result(3);
    result.push_back(1);
    result.push_back(2);
    result.push_back(3);
    result.push_back(4);
    result.push_back(5);
    result.push_back(6);
    CCirtucalBuffer<int> expect = {4, 5, 6};

    ASSERT_EQ(result, expect);
}

TEST(CirtucalBufferTestSuite, PushFrontTest) {
    CCirtucalBuffer<int> result(3);
    result.push_front(1);
    result.push_front(2);
    result.push_front(3);
    result.push_front(4);
    result.push_front(5);
    result.push_front(6);

    CCirtucalBuffer<int> expect = {6, 5, 4};

    ASSERT_EQ(result, expect);
}

TEST(CirtucalBufferTestSuite, PopFrontTest) {
    CCirtucalBuffer<int> result({1, 2, 3, 4, 5, 6});
    ASSERT_EQ(result.front(), 1);
    result.pop_front();
    ASSERT_EQ(result.front(), 2);
    result.pop_front();
    ASSERT_EQ(result.front(), 3);
    result.pop_front();


    CCirtucalBuffer<int> expect = {4, 5, 6};
    ASSERT_EQ(result, expect);
}

TEST(CirtucalBufferTestSuite, PopBackTest) {
    CCirtucalBuffer<int> result({1, 2, 3, 4, 5, 6});
    ASSERT_EQ(result.back(), 6);
    result.pop_back();
    ASSERT_EQ(result.back(), 5);
    result.pop_back();
    ASSERT_EQ(result.back(), 4);
    result.pop_back();


    CCirtucalBuffer<int> expect = {1, 2, 3};
    ASSERT_EQ(result, expect);
}

TEST(CirtucalBufferTestSuite, EmptyTest) {
    CCirtucalBuffer<int> result(3);
    ASSERT_TRUE(result.empty());

    result.push_back(3);
    result.push_back(3);
    result.push_back(3);
    result.push_back(3);
    result.push_back(3);

    result.pop_front();
    result.pop_front();
    result.pop_front();

    ASSERT_TRUE(result.empty());
}

TEST(CirtucalBufferTestSuite, FullTest) {
    CCirtucalBuffer<int> result(3);
    ASSERT_FALSE(result.full());

    result.push_back(3);
    result.push_back(3);
    result.push_back(3);
    ASSERT_TRUE(result.full());

    result.push_back(3);
    result.push_back(3);
    ASSERT_TRUE(result.full());

    result.pop_front();
    ASSERT_FALSE(result.full());
}

TEST(CirtucalBufferTestSuite, ClearTest) {
    CCirtucalBuffer<int> result(3);

    result.push_back(3);
    ASSERT_FALSE(result.empty());

    result.clear();
    ASSERT_TRUE(result.empty());

    result.push_back(3);
    result.push_back(3);
    result.push_back(3);
    result.push_back(3);
    result.push_back(3);
    ASSERT_FALSE(result.empty());

    result.clear();
    ASSERT_TRUE(result.empty());
}

TEST(CirtucalBufferTestSuite, GetSizeTest) {
    CCirtucalBuffer<int> result(5);

    result.push_back(3);
    result.push_back(3);
    result.push_back(3);
    ASSERT_EQ(result.size(), 3);

    result.pop_front();
    result.pop_front();
    ASSERT_EQ(result.size(), 1);

    result.push_back(3);
    result.push_back(3);
    result.push_back(3);
    result.push_back(3);
    result.push_back(3);
    ASSERT_EQ(result.size(), result.capacity()); // 5
}

TEST(CirtucalBufferTestSuite, RandomAtTest) {
    CCirtucalBuffer<int> result(5);

    result.push_back(1);
    result.push_back(2);
    result.push_back(3);
    ASSERT_EQ(result.at(2), 3);
    ASSERT_EQ(result[2], 3);

    result.pop_front();
    result.pop_front();
    result.push_back(4);
    result.push_back(5);
    ASSERT_EQ(result.at(1), 4);
    ASSERT_EQ(result[1], 4);

    result.push_back(6);
    result.push_back(7);
    result.push_back(8);
    result.push_back(9);
    result.push_back(10);
    ASSERT_EQ(result.at(1), 7);
    ASSERT_EQ(result.at(3), 9);
    ASSERT_EQ(result[3], 9);

    ASSERT_THROW(result.at(5), std::out_of_range);
    ASSERT_THROW(result[5], std::out_of_range);
}

TEST(CirtucalBufferTestSuite, EraseTest) {
    CCirtucalBuffer<int> buff1({1, 2, 1, 2, 1, 7});

    CCirtucalBuffer<int>::iterator it = std::remove(buff1.begin(), buff1.end(), 1);
    buff1.erase(it, buff1.end());

    CCirtucalBuffer<int> except1({2, 2, 7});
    ASSERT_EQ(buff1, except1);


    CCirtucalBuffer<int> buff2({1, 2, 3, 4, 5});
    buff2.erase(buff2.begin() + 2);

    CCirtucalBuffer<int> except2({1, 2, 4, 5});
    ASSERT_EQ(buff2, except2);


    CCirtucalBuffer<int> buff3({1, 2, 3, 4, 5, 6, 7});
    buff3.erase(buff3.begin() + 2, buff3.end() - 2);

    CCirtucalBuffer<int> except3({1, 2, 6, 7});
    ASSERT_EQ(buff3, except3);
}


TEST(CirtucalBufferTestSuite, EmplaceTest) {
    CCirtucalBuffer<HardTestObj> buff1(3);

    buff1.emplace_back(1);
    buff1.emplace_back(2, true);
    buff1.emplace_back();
    buff1.emplace_back(3);

    ASSERT_EQ(buff1.at(0).val1, 2);
    ASSERT_TRUE(buff1.at(0).val2);

    ASSERT_EQ(buff1.at(1).val1, 0);
    ASSERT_FALSE(buff1.at(1).val2);

    ASSERT_EQ(buff1.at(2).val1, 3);
    ASSERT_FALSE(buff1.at(1).val2);
}

TEST(CirtucalBufferTestSuite, ResizeTest) {
    CCirtucalBuffer<HardTestObj> buff({HardTestObj(1), HardTestObj(2), HardTestObj(3)});
    ASSERT_EQ(buff.size(), 3);
    ASSERT_EQ(buff.capacity(), 3);

    buff.resize(5);
    ASSERT_EQ(buff.size(), 5);
    ASSERT_EQ(buff.capacity(), 5);

    ASSERT_EQ(buff.at(3).val1, 0);
    ASSERT_EQ(buff.at(4).val1, 0);

    buff.resize(2);
    ASSERT_EQ(buff.size(), 2);
    ASSERT_EQ(buff.capacity(), 5);
}

TEST(CirtucalBufferTestSuite, AssignTest) {
    std::initializer_list<int> il = {1, 2, 3, 4, 5};

    CCirtucalBuffer<int> buff1(10);
    buff1.push_back(0);
    buff1.push_back(0);
    ASSERT_EQ(buff1.size(), 2);
    ASSERT_EQ(buff1.capacity(), 10);

    buff1.assign(il);
    ASSERT_EQ(buff1.size(), 5);
    ASSERT_EQ(buff1.capacity(), 10);
    ASSERT_EQ(buff1.at(2), 3);


    CCirtucalBuffer<int> buff2(3);
    buff2.push_back(0);
    buff2.push_back(0);
    ASSERT_EQ(buff2.size(), 2);
    ASSERT_EQ(buff2.capacity(), 3);

    buff2.assign(il.begin(), il.end());
    ASSERT_EQ(buff2.size(), 5);
    ASSERT_EQ(buff2.capacity(), 5);
    ASSERT_EQ(buff2.at(2), 3);
}

TEST(CirtucalBufferTestSuite, InsertTest) {
    std::initializer_list<int> il = {47, 78};

    CCirtucalBuffer<int> buff({1, 2, 3});

    buff.insert(2, 10);
    ASSERT_EQ(buff, CCirtucalBuffer<int>({1, 2, 10, 3}));

    buff.insert(3, 3, 5);
    ASSERT_EQ(buff, CCirtucalBuffer<int>({1, 2, 10, 5, 5, 5, 3}));

    buff.insert(2, il);
    ASSERT_EQ(buff, CCirtucalBuffer<int>({1, 2, 47, 78, 10, 5, 5, 5, 3}));

    buff.insert(7, il.begin(), il.end());
    ASSERT_EQ(buff, CCirtucalBuffer<int>({1, 2, 47, 78, 10, 5, 5, 47, 78, 5, 3}));
}

TEST(CirtucalBufferTestSuite, OverwriteTest) {
    std::initializer_list<int> il = {47, 78};

    CCirtucalBuffer<int> buff({1, 2, 3, 4, 5, 6, 7});

    buff.overwrite(4, 10);
    ASSERT_EQ(buff, CCirtucalBuffer<int>({1, 2, 3, 4, 10, 6, 7}));

    buff.overwrite(3, 3, 23);
    ASSERT_EQ(buff, CCirtucalBuffer<int>({1, 2, 3, 23, 23, 23, 7}));

    buff.overwrite(2, il);
    ASSERT_EQ(buff, CCirtucalBuffer<int>({1, 2, 47, 78, 23, 23, 7}));

    buff.overwrite(0, il.begin(), il.end());
    ASSERT_EQ(buff, CCirtucalBuffer<int>({47, 78, 47, 78, 23, 23, 7}));
}

TEST(CirtucalBufferTestSuite, SortTest) {
    CCirtucalBuffer<int> buff({451, 5431, 5, 657, -32, 769, 4643, 6, 76465, 10, -2532});

    std::sort(buff.begin(), buff.end());
    ASSERT_EQ(buff, CCirtucalBuffer<int>({-2532, -32, 5, 6, 10, 451, 657, 769, 4643, 5431, 76465}));
}


TEST(CirtucalBufferTestSuite, BufferExtTest) {
    CCirtucalBufferExt<int> buff({1, 2, 3});

    ASSERT_EQ(buff.size(), 3);
    ASSERT_EQ(buff.capacity(), 3);

    buff.push_back(4);
    ASSERT_EQ(buff.size(), 4);
    ASSERT_EQ(buff.capacity(), 6);

    buff.push_front(5);
    buff.push_front(6);
    buff.push_front(7);
    ASSERT_EQ(buff.size(), 7);
    ASSERT_EQ(buff.capacity(), 12);

    ASSERT_EQ(buff, CCirtucalBufferExt<int>({7, 6, 5, 1, 2, 3, 4}));
}

#include <gtest/gtest.h>
#include "deque.hpp"  


class DequeTest: public testing::Test {
  protected:
    void SetUp() override {
      for (size_t i = 0; i < sz; ++i) {
        dq.PushBack(i);
      }
      assert(dq.Size() == sz);
    }

  Deque<int> dq;
  const size_t sz = 7;
};

TEST(EmptyDequeTest, DefaultConstructor) {
    Deque<int> deque;
    ASSERT_TRUE(deque.IsEmpty());
    ASSERT_EQ(deque.Size(), 0);
}

TEST(EmptyDequeTest, AssignIntConstructor) {
    Deque<int> deque(10, 5);
    ASSERT_EQ(deque.Size(), 10);
    for (auto it = deque.Begin(); it != deque.End(); ++it) {
        ASSERT_EQ(deque[*it], 5);
    }
}

TEST(EmptyDequeTest, PushBackSimple) {
    Deque<int> deque;
    deque.PushBack(1);
    deque.PushBack(2);
    ASSERT_EQ(deque.Size(), 2);
    ASSERT_EQ(deque.Back(), 2);
    ASSERT_FALSE(deque.IsEmpty());

}

TEST(EmptyDequeTest, PushFrontSimple) {
    Deque<int> deque;
    deque.PushFront(1);
    deque.PushFront(2);
    ASSERT_EQ(deque.Size(), 2);
    ASSERT_EQ(deque.Front(), 2);
    ASSERT_FALSE(deque.IsEmpty());

}

TEST(EmptyDequeTest, PopFrontSimple) {
    Deque<int> deque;
    for (int i = 0; i < 10; ++i) {
        deque.PushBack(i);
    }
    for (auto it = deque.Begin(); *it != 5; ++it) {
        deque.PopFront();
    }
    ASSERT_EQ(deque.Size(), 5);
    for (int i = 0; i < 5; ++i) {
        ASSERT_EQ(deque[i], i + 5);
    }
}

TEST(EmptyDequeTest, PopBackSimple) {
    Deque<int> deque;
    for (int i = 0; i < 10; ++i) {
        deque.PushBack(i);
    }
    for (int i = 0; i < 5; ++i) {
        deque.PopBack();
    }
    ASSERT_EQ(deque.Size(), 5);
    for (int i = 0; i < 5; ++i) {
        ASSERT_EQ(deque[i], i);
    }
}

TEST(EmptyDequeTest, PopEmptyDeque) {
    Deque<int> deque;
    EXPECT_THROW({
    deque.PopFront();
    }, DequeIsEmptyException);
    EXPECT_THROW({
    deque.PopBack();
    }, DequeIsEmptyException);

}
TEST(RandomDequeTest, InitializerListConstructor) {
    Deque<int> deque{1, 2, 3, 4, 5};
    ASSERT_EQ(deque.Size(), 5);

    auto it = deque.Begin();
    ASSERT_EQ(*it, 1);
    ++it;
    ASSERT_EQ(*it, 2);
    ++it;
    ASSERT_EQ(*it, 3);
    ++it;
    ASSERT_EQ(*it, 4);
    ++it;
    ASSERT_EQ(*it, 5);
}

TEST(RandomDequeTest, EmptyInitializerListConstructor) {
    Deque<int> deque{};
    ASSERT_EQ(deque.Size(), 0);
    ASSERT_TRUE(deque.IsEmpty());
}


TEST_F(DequeTest, CopyConstructor) {
    Deque<int> deque(dq);
    ASSERT_EQ(deque.Size(), sz);
    for (auto it = deque.Begin(); it != deque.End(); ++it) {
        ASSERT_EQ(deque[*it], *it);
    }
}


TEST_F(DequeTest, MoveConstructor) {
    Deque<int> deque(std::move(dq));
    ASSERT_EQ(deque.Size(), sz);
    for (auto it = deque.Begin(); it != deque.End(); ++it) {
        ASSERT_EQ(deque[*it], *it);
    }
    ASSERT_TRUE(dq.IsEmpty());
}


TEST_F(DequeTest, CopyAssignment) {
    Deque<int> deque;
    deque = dq;
    ASSERT_EQ(deque.Size(), sz);
    for (auto it = deque.Begin(); it != deque.End(); ++it) {
        ASSERT_EQ(deque[*it], *it);
    }
}


TEST_F(DequeTest, MoveAssignment) {
    Deque<int> deque;
    deque = std::move(dq);
    ASSERT_EQ(deque.Size(), sz);
    for (auto it = deque.Begin(); it != deque.End(); ++it) {
        ASSERT_EQ(deque[*it], *it);
    }
    ASSERT_TRUE(dq.IsEmpty());
}

TEST_F(DequeTest, Clear) {
    dq.Clear();
    ASSERT_TRUE(dq.IsEmpty());
}

TEST_F(DequeTest, PopBack) {
    for (size_t i = 0; i < 5; ++i) {
        dq.PopBack();
    }
    ASSERT_EQ(dq.Size(), 2);
    for (size_t i = 0; i < 2; ++i) {
        ASSERT_EQ(dq[i], i);
    }
}

TEST(RandomDequeTest, PushAtBlockBoundaries) {
    Deque<int> deque;
    for (size_t i = 0; i < DEFAULT_SIZE_BLOCK; ++i) {
        deque.PushBack(i);
    }
    ASSERT_EQ(deque.Size(), DEFAULT_SIZE_BLOCK);
    deque.PushBack(DEFAULT_SIZE_BLOCK);
    ASSERT_EQ(deque.Size(), DEFAULT_SIZE_BLOCK + 1);
    ASSERT_EQ(deque[DEFAULT_SIZE_BLOCK], DEFAULT_SIZE_BLOCK);
    for (size_t i = 0; i < DEFAULT_SIZE_BLOCK + 1; ++i) {
        ASSERT_EQ(deque[i], i);
    }
}

struct CustomType {
    int value;
    CustomType(int v) : value(v) {}
    bool operator==(const CustomType& other) const {
        return value == other.value;
    }
};

TEST(RandomDequeTest, CustomType) {
    Deque<CustomType> deque;
    for (int i = 0; i < 10; ++i) {
        deque.PushBack(CustomType(i));
    }
    ASSERT_EQ(deque.Size(), 10);
    for (int i = 0; i < 10; ++i) {
        ASSERT_EQ(deque[i], CustomType(i));
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

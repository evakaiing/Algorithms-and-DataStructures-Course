#include <gtest/gtest.h>
#include "../Queue.hpp"
#include <fmt/core.h>
#include <deque>
#include <vector>
#include <list>

using namespace std;


class QueueTest: public testing::Test {
    protected:
      void SetUp() override {
        qu.Push(1);
        qu.Push(2);
        qu.Push(3);
        qu.Push(4);
        qu.Push(5);
        qu.Push(6);
        qu.Push(7);
        assert(qu.Size() == sz);
      }
      Queue<int> qu;
      const size_t sz = 7;
};

TEST(EmptyQueueTest, DefaultConstructor) {
    Queue<int> qu;
    ASSERT_TRUE(qu.Empty()) << "Default Queue isn't empty";
}

TEST(EmptyQueueTest, PushSimple) {
    Queue<int> qu;
    qu.Push(5);
    ASSERT_EQ(qu.Size(), 1);
    ASSERT_EQ(qu.Front(), 5);
}

TEST(EmptyQueueTest, PopSimple) {
    Queue<int> qu;
    qu.Push(5);
    qu.Push(1);
    qu.Pop();

    ASSERT_EQ(qu.Size(), 1);
    ASSERT_EQ(qu.Front(), 1);
}


TEST(EmptyQueueTest, PopEmptyQueue) {
    Queue<int> qu;
    EXPECT_THROW({
    qu.Pop();
    }, QueueIsEmptyException);
}

TEST(EmptyQueueTest, PopOneElement) {
    Queue<int> qu;
    qu.Push(1);
    qu.Pop();

    ASSERT_TRUE(qu.Empty());
}

TEST(EmptyQueueTest, TopEmptyQueue) {
    Queue<int> qu;
    EXPECT_THROW({
    qu.Front();
    }, QueueIsEmptyException);
}

TEST_F(QueueTest, CopyConstructorOtherQueue) {
    Queue<int> qu2 = qu;
    ASSERT_NE(&qu, &qu2);
    ASSERT_EQ(qu.Size(), qu2.Size());
    while (!qu2.Empty()) {
        ASSERT_EQ(qu2.Front(), qu.Front());
        qu.Pop();
        qu2.Pop();
    }
}

TEST_F(QueueTest, CopyAssigmentOtherQueue) {
    Queue<int> qu2;
    qu2.Push(4);
    qu = qu2;
    ASSERT_NE(&qu, &qu2);
    ASSERT_EQ(qu.Size(), qu2.Size());
    while (!qu2.Empty()) {
        ASSERT_EQ(qu2.Front(), qu.Front());
        qu.Pop();
        qu2.Pop();
    }
}

TEST_F(QueueTest, MoveConstructorOtherQueue) {
    Queue<int> qu2 = std::move(qu);
    ASSERT_EQ(sz, qu2.Size());
    size_t i = 0;
    while (!qu2.Empty()) {
        ASSERT_EQ(qu2.Front(), i + 1);
        qu2.Pop();
        ++i;
    }
}

TEST_F(QueueTest, MoveOperatorOtherQueue) {
    Queue<int> qu2;
    qu2.Push(4);
    qu = std::move(qu2);
    ASSERT_EQ(qu.Size(), 1);
    ASSERT_EQ(qu2.Size(), 0);
}

TEST(QueueTestContainer, CopyConstuctorBasedOnContainer) {
    std::vector<int> vec = {1, 2, 3, 4, 5};
    Queue<int, std::vector<int>> qu(vec);
    ASSERT_EQ(qu.Size(), vec.size());
    ASSERT_EQ(qu.Front(), 1);
}

TEST(QueueTestContainer, MoveConstructorBasedOnConatiner) {
    std::list<int> lst = {1, 2, 3, 4, 5};
    Queue<int, std::list<int>> qu(std::move(lst));
    ASSERT_EQ(qu.Size(), 5);
    ASSERT_EQ(qu.Front(), 1);
}


int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}

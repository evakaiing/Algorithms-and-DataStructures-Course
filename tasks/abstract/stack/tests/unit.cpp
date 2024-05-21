#include <gtest/gtest.h>
#include "../stack.hpp"
#include <fmt/core.h>
#include <deque>
#include <vector>
#include <list>

using namespace std;


class StackTest: public testing::Test {
    protected:
      void SetUp() override {
        st.Push(1);
        st.Push(2);
        st.Push(3);
        st.Push(4);
        st.Push(5);
        st.Push(6);
        st.Push(7);
        assert(st.Size() == sz);
      }
      Stack<int> st;
      const size_t sz = 7;
};

TEST(EmptyStackTest, DefaultConstructor) {
    Stack<int> st;
    ASSERT_TRUE(st.Empty()) << "Default stack isn't empty";
}

TEST(EmptyStackTest, PushSimple) {
    Stack<int> st;
    st.Push(5);
    ASSERT_EQ(st.Size(), 1);
    ASSERT_EQ(st.Top(), 5);
}

TEST(EmptyStackTest, PopSimple) {
    Stack<int> st;
    st.Push(5);
    st.Push(1);
    st.Pop();

    ASSERT_EQ(st.Size(), 1);
    ASSERT_EQ(st.Top(), 5);
}


TEST(EmptyStackTest, PopEmptyStack) {
    Stack<int> st;
    EXPECT_THROW({
    st.Pop();
    }, StackIsEmptyException);
}

TEST(EmptyStackTest, PopOneElement) {
    Stack<int> st;
    st.Push(1);
    st.Pop();

    ASSERT_TRUE(st.Empty());
}

TEST(EmptyStackTest, TopEmptyStack) {
    Stack<int> st;
    EXPECT_THROW({
    st.Top();
    }, StackIsEmptyException);
}

TEST_F(StackTest, CopyConstructorOtherStack) {
    Stack<int> st2 = st;
    ASSERT_NE(&st, &st2);
    ASSERT_EQ(st.Size(), st2.Size());
    while (!st2.Empty()) {
        ASSERT_EQ(st2.Top(), st.Top());
        st.Pop();
        st2.Pop();
    }
}

TEST_F(StackTest, CopyAssigmentOtherStack) {
    Stack<int> st2;
    st2.Push(4);
    st = st2;
    ASSERT_NE(&st, &st2);
    ASSERT_EQ(st.Size(), st2.Size());
    while (!st2.Empty()) {
        ASSERT_EQ(st2.Top(), st.Top());
        st.Pop();
        st2.Pop();
    }
}

TEST_F(StackTest, MoveConstructorOtherStack) {
    Stack<int> st2 = std::move(st);
    ASSERT_EQ(sz, st2.Size());
    size_t i = sz;
    while (!st2.Empty()) {
        ASSERT_EQ(st2.Top(), i);
        st2.Pop();
        --i;
    }
}

TEST_F(StackTest, MoveOperatorOtherStack) {
    Stack<int> st2;
    st2.Push(4);
    st = std::move(st2);
    ASSERT_EQ(st.Size(), 1);
    ASSERT_EQ(st2.Size(), 0);
}

TEST(StackTestContainer, CopyConstuctorBasedOnContainer) {
    std::vector<int> vec = {1, 2, 3, 4, 5};
    Stack<int, std::vector<int>> st(vec);
    ASSERT_EQ(st.Size(), vec.size());
    ASSERT_EQ(st.Top(), 5);
}

TEST(StackTestContainer, MoveConstructorBasedOnConatiner) {
    std::list<int> lst = {1, 2, 3, 4, 5};
    Stack<int, std::list<int>> st(std::move(lst));
    ASSERT_EQ(st.Size(), 5);
    ASSERT_EQ(st.Top(), 5);
}


int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}

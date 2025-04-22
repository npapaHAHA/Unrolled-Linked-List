#include <unrolled_list.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <vector>
#include <list>

/*
    В данном файле представлен ряд тестов, где используются (вместе, раздельно и по-очереди):
        - push_back
        - push_front
        - insert
    Методы применяются на unrolled_list и на std::list.
    Ожидается, что в итоге порядок элементов в контейнерах будут идентичен
*/

TEST(UnrolledLinkedList, pushBack) {
    std::list<int> std_list;
    unrolled_list<int> unrolled_list;

    for (int i = 0; i < 1000; ++i) {
        std_list.push_back(i);
        unrolled_list.push_back(i);
    }

    ASSERT_THAT(unrolled_list, ::testing::ElementsAreArray(std_list));
}

TEST(UnrolledLinkedList, pushFront) {
    std::list<int> std_list;
    unrolled_list<int> unrolled_list;

    for (int i = 0; i < 1000; ++i) {
        std_list.push_front(i);
        unrolled_list.push_front(i);
    }

    ASSERT_THAT(unrolled_list, ::testing::ElementsAreArray(std_list));
}

TEST(UnrolledLinkedList, pushMixed) {
    std::list<int> std_list;
    unrolled_list<int> unrolled_list;

    for (int i = 0; i < 1000; ++i) {
        if (i % 2 == 0) {
            std_list.push_front(i);
            unrolled_list.push_front(i);
        } else {
            std_list.push_back(i);
            unrolled_list.push_back(i);
        }
    }

    ASSERT_THAT(unrolled_list, ::testing::ElementsAreArray(std_list));
}

TEST(UnrolledLinkedList, insertAndPushMixed) {
    std::list<int> std_list;
    unrolled_list<int> unrolled_list;

    for (int i = 0; i < 1000; ++i) {
        if (i % 3 == 0) {
            std_list.push_front(i);
            unrolled_list.push_front(i);
        } else if (i % 3 == 1) {
            std_list.push_back(i);
            unrolled_list.push_back(i);
        } else {
            auto std_it = std_list.begin();
            auto unrolled_it = unrolled_list.begin();
            std::advance(std_it, std_list.size() / 2);
            std::advance(unrolled_it, std_list.size() / 2);
            std_list.insert(std_it, i);
            unrolled_list.insert(unrolled_it, i);
        }
    }

    ASSERT_THAT(unrolled_list, ::testing::ElementsAreArray(std_list));
}

TEST(UnrolledLinkedList, popFrontBack) {
    std::list<int> std_list;
    unrolled_list<int> unrolled_list;

    for (int i = 0; i < 1000; ++i) {
        std_list.push_back(i);
        unrolled_list.push_back(i);
    }

    for (int i = 0; i < 500; ++i) {
        if (i % 2 == 0) {
            std_list.pop_back();
            unrolled_list.pop_back();
        } else {
            std_list.pop_front();
            unrolled_list.pop_front();
        }
    }

    ASSERT_THAT(unrolled_list, ::testing::ElementsAreArray(std_list));

    for (int i = 0; i < 500; ++i) {
        std_list.pop_back();
        unrolled_list.pop_back();
    }

    ASSERT_TRUE(unrolled_list.empty());
}

TEST(UnrolledLinkedList, clearAndSize) {
    unrolled_list<int> unrolled_list;
    for (int i = 0; i < 100; ++i) {
        unrolled_list.push_back(i);
    }
    ASSERT_EQ(unrolled_list.size(), 100);

    unrolled_list.clear();
    ASSERT_EQ(unrolled_list.size(), 0);
    ASSERT_TRUE(unrolled_list.empty());
}

TEST(UnrolledLinkedList, frontAndBack) {
    unrolled_list<int> unrolled_list;
    unrolled_list.push_back(10);
    unrolled_list.push_back(20);
    unrolled_list.push_back(30);

    ASSERT_EQ(unrolled_list.front(), 10);
    ASSERT_EQ(unrolled_list.back(), 30);
}

TEST(UnrolledLinkedList, reverseIteration) {
    unrolled_list<int> list;
    std::vector<int> expected;

    for (int i = 0; i < 100; ++i) {
        list.push_back(i);
        expected.push_back(i);
    }

    ASSERT_THAT(std::vector<int>(list.rbegin(), list.rend()), ::testing::ElementsAreArray(std::vector<int>(expected.rbegin(), expected.rend())));
}

TEST(UnrolledLinkedList, copyConstructorAndEquality) {
    unrolled_list<int> list1;
    for (int i = 0; i < 50; ++i) list1.push_back(i);

    unrolled_list<int> list2(list1);
    ASSERT_TRUE(list1 == list2);

    list2.pop_back();
    ASSERT_TRUE(list1 != list2);
}

TEST(UnrolledLinkedList, AssignInitializerList) {
    unrolled_list<int> u;

    u.assign({10, 20, 30, 40});

    ASSERT_EQ(u.size(), 4);
    ASSERT_THAT(u, ::testing::ElementsAre(10, 20, 30, 40));

    u.assign({1, 2});

    ASSERT_EQ(u.size(), 2);
    ASSERT_THAT(u, ::testing::ElementsAre(1, 2));
}

TEST(UnrolledLinkedList, AssignNCopies) {
    unrolled_list<std::string> u;

    u.assign(3, "hello");

    ASSERT_EQ(u.size(), 3);
    ASSERT_THAT(u, ::testing::ElementsAre("hello", "hello", "hello"));

    u.assign(0, "empty");
    ASSERT_TRUE(u.empty());
}

TEST(UnrolledLinkedList, AssignFromIterators) {
    unrolled_list<int> u;

    std::vector<int> vec = {1, 3, 5, 7};
    u.assign(vec.begin(), vec.end());

    ASSERT_EQ(u.size(), vec.size());
    ASSERT_THAT(u, ::testing::ElementsAreArray(vec));
}


//////////////////////////////////////////////////



#include <unrolled_list.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

struct NoDefaultConstructible {
    NoDefaultConstructible() = delete;

    NoDefaultConstructible(int) {}
};

/*
    Тест проверяет, что ваш контейнер умеет работать с типами, у которых нет конструктора по-умолчанию
*/
TEST(NoDefaultConstructible, canConstruct) {
    unrolled_list<NoDefaultConstructible> unrolled_list;
    unrolled_list.push_front(NoDefaultConstructible(1));
    unrolled_list.push_back(NoDefaultConstructible(2));

    ASSERT_EQ(unrolled_list.size(), 2);
}

TEST(NoDefaultConstructible, insertWorksCorrectly) {
    unrolled_list<NoDefaultConstructible> unrolled_list;
    unrolled_list.push_back(NoDefaultConstructible(1));
    unrolled_list.push_back(NoDefaultConstructible(3));

    auto it = unrolled_list.begin();
    ++it;

    unrolled_list.insert(it, NoDefaultConstructible(2));

    ASSERT_EQ(unrolled_list.size(), 3);

    int expected_val = 1;
    for (auto it = unrolled_list.begin(); it != unrolled_list.end(); ++it) {
        (void)*it;
        ++expected_val;
    }
}

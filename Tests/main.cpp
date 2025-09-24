#include <iostream>
#include <windows.h>
#include <sstream>

#include "tvector.h"


void set_color(int text_color, int bg_color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, (bg_color << 4) | text_color);
}

namespace TestSystem {
    int count_success = 0, count_failed = 0;

    void start_test(bool(*test)(), const char* name_of_test) {
        set_color(2, 0);
        std::cout << "[ RUN      ]";
        set_color(7, 0);
        std::cout << name_of_test << std::endl;

        bool status = test();

        if (status == true) {
            set_color(2, 0);
            std::cout << "[       OK ]" << std::endl;
            count_success++;
        }
        else {
            set_color(4, 0);
            std::cout << "[  FAILED  ]" << std::endl;
            count_failed++;
        }
        set_color(7, 0);
    }

    template <class T>
    bool check(const T& expected, const T& actual) {
        if (expected == actual) {
            return true;
        }
        else {
            std::cerr << "Expected result is " << expected << ", but actual is " << actual << "." << std::endl;
            return false;
        }
    }

    void print_init_info() {
        set_color(2, 0);
        std::cout << "[==========] " << std::endl;
        set_color(7, 0);
    }

    void print_final_info() {
        set_color(2, 0);
        std::cout << "[==========] ";
        set_color(7, 0);
        std::cout << count_success + count_failed << " test" << (count_success + count_failed > 1 ? "s" : "") << " ran." << std::endl;
        set_color(2, 0);
        std::cout << "[  PASSED  ] ";
        set_color(7, 0);
        std::cout << count_success << " test" << (count_success > 1 ? "s" : "") << std::endl;
        if (count_failed > 0) {
            set_color(4, 0);
            std::cout << "[  FAILED  ] ";
            set_color(7, 0);
            std::cout << count_failed << " test" << (count_failed > 1 ? "s." : ".") << std::endl;
        }
    }
};

template <class T>
class TVectorTester {
public:
    static bool test_is_full(const TVector<T>& vec) { // static чтобы вызывать методы без создания объекта класса TVectorTester<T>
        return vec.is_full();
    }
};


bool test_1_default_constructor() {
    TVector<int> vec;
    size_t expected_result_size = 0;
    size_t actual_result_size = vec.size();
    size_t expected_result_capacity = 0;
    size_t actual_result_capacity = vec.capacity();
    return TestSystem::check(expected_result_size, actual_result_size) &&
        TestSystem::check(expected_result_capacity, actual_result_capacity);
}
bool test_2_constructor_with_size() {
    size_t n = 10;
    TVector<int> vec(n);
    const State* states_vec = vec.states();

    bool actual_result_states = true;
    bool expected_result_states = true;
    for (size_t i = 0; i < n; ++i) {
        if (states_vec[i] != busy) {
            actual_result_states = false;
            break;
        }
    }

    size_t expected_result_size = n;
    size_t actual_result_size = vec.size();
    size_t expected_result_capacity = n + TVector<int>::RESERVE_MEMORY;
    size_t actual_result_capacity = vec.capacity();
    return TestSystem::check(expected_result_size, actual_result_size) &&
        TestSystem::check(expected_result_capacity, actual_result_capacity) &&
        TestSystem::check(expected_result_states, actual_result_states);
}
bool test_3_constructor_with_mass() {
    int copy_mass[3] = { 1, 2, 3 };
    TVector<int> vec(copy_mass, 3);
    const State* states_vec = vec.states();
    const int* data_vec = vec.data();

    bool actual_result_mass = true;
    bool expected_result_mass = true;
    for (size_t i = 0; i < 3; ++i) {
        if (states_vec[i] != busy || data_vec[i] != copy_mass[i]) {
            actual_result_mass = false;
            break;
        }
    }
    size_t expected_result_size = 3;
    size_t actual_result_size = vec.size();
    size_t expected_result_capacity = 3 + TVector<int>::RESERVE_MEMORY;
    size_t actual_result_capacity = vec.capacity();

    return TestSystem::check(expected_result_size, actual_result_size) &&
        TestSystem::check(expected_result_capacity, actual_result_capacity) &&
        TestSystem::check(expected_result_mass, actual_result_mass);
}
bool test_4_constructor_copy() {
    TVector<int> original_vec(7);
    TVector<int> copy_vec(original_vec);

    const State* states_original_vec = original_vec.states();
    const int* data_original_vec = original_vec.data();
    const State* states_copy_vec = copy_vec.states();
    const int* data_copy_vec = copy_vec.data();
    bool actual_result_mass = true;
    bool expected_result_mass = true;
    for (size_t i = 0; i < 7; ++i) {
        if (states_original_vec[i] != states_copy_vec[i]
            || data_original_vec[i] != data_copy_vec[i]) {
            actual_result_mass = false;
            break;
        }
    }

    return TestSystem::check(original_vec.size(), copy_vec.size()) &&
        TestSystem::check(original_vec.capacity(), copy_vec.capacity()) &&
        TestSystem::check(expected_result_mass, actual_result_mass);
}
bool test_5_at() {
    TVector<int> vec(7);
    vec[0] = 63;
    int actual_result = vec.at(0);
    int expected_result = 63;
    return TestSystem::check(expected_result, actual_result);
}
bool test_6_front() {
    TVector<int> vec(3);
    vec[0] = 112;
    vec[1] = 234;
    vec[2] = 345;

    int front_expected_result = 112;
    int front_actual_result = vec.front();

    return TestSystem::check(front_expected_result, front_actual_result);
}
bool test_7_back() {
    TVector<int> vec(3);
    vec[0] = 100;
    vec[1] = 200;
    vec[2] = 369;

    int back_expected_result = 369;
    int back_actual_resualt = vec.back();

    return TestSystem::check(back_expected_result, back_actual_resualt);
}
bool test_8_begin_and_end() {
    TVector<int> vec(3);
    vec[0] = 11;
    vec[1] = 22;
    vec[2] = 37;

    int* begin = vec.begin();
    int* end = vec.end();
    int sum = 0;

    for (int* p = begin; p < end; ++p)
        sum += *p;

    int expected_resualt = 70;
    return TestSystem::check(expected_resualt, sum);
}
bool test_9_assign() {
    TVector<int> vec1(3);
    vec1[0] = 1;
    vec1[1] = 2;
    vec1[2] = 3;

    TVector<int> vec2;
    vec2.assign(vec1);

    return TestSystem::check(vec1.size(), vec2.size()) &&
        TestSystem::check(vec1[0], vec2[0]) &&
        TestSystem::check(vec1[1], vec2[1]) &&
        TestSystem::check(vec1[2], vec2[2]);
}
bool test_10_operator_equal() {
    TVector<int> vec1(2);
    vec1[0] = 5;
    vec1[1] = 6;

    TVector<int> vec2;
    vec2 = vec1;
    bool expected_result_size = true;
    bool actual_result_size = true;
    if (vec1.size() != vec2.size()) {
        actual_result_size = false;
    }
    bool expected_result_values = true;
    bool actual_result_values = true;
    for (size_t i = 0; i < 2; ++i) {
        if (vec1[i] != vec2[i]) {
            actual_result_values = false;
            break;
        }
    }
    return TestSystem::check(expected_result_size, actual_result_size) &&
        TestSystem::check(expected_result_values, actual_result_values);
}
bool test_11_clear() {
    TVector<int> vec(5);
    const State* vec_states = vec.states();
    vec.clear();
    bool actual_result_states = true;
    bool expected_result_states = true;
    for (size_t i = 0; i < 5; ++i) {
        if (vec_states[i] != State::empty) {
            actual_result_states = false;
            break;
        }
    }
    bool actual_result_size = true;
    bool expected_result_size = true;
    if (vec.size() != 0) {
        actual_result_size = false;
    }
    bool actual_result_deleted = true;
    bool expected_result_deleted = true;
    if (vec.size() != 0) {
        actual_result_deleted = false;
    }
    return TestSystem::check(expected_result_states, actual_result_states)
        && TestSystem::check(expected_result_size, actual_result_size)
        && TestSystem::check(expected_result_deleted, actual_result_deleted);
}
bool test_12_operator_equal_equal() {
    TVector<int> vec1(3);
    vec1[0] = 5;
    vec1[1] = 6;
    vec1[2] = 7;

    TVector<int> vec2(3);
    vec2[0] = 5;
    vec2[1] = 6;
    vec2[2] = 7;

    bool excepted_result = true;
    bool actual_result = (vec1 == vec2);
    return TestSystem::check(excepted_result, actual_result);
}
bool test_13_operator_not_equal() {
    TVector<int> vec1(3);
    vec1[0] = 5;
    vec1[1] = 6;
    vec1[2] = 7;

    TVector<int> vec2(3);
    vec2[0] = 5;
    vec2[1] = 6;
    vec2[2] = 7;

    bool excepted_result = false;
    bool actual_result = (vec1 != vec2);
    return TestSystem::check(excepted_result, actual_result);
}

bool test_14_is_full() {
    TVector<int> vec(2);
    vec[0] = 42;
    vec[1] = 99;
    bool excepted_result = false;
    bool actual_result = TVectorTester<int>::test_is_full(vec);
    return TestSystem::check(excepted_result, actual_result);
}
bool test_15_reserve_new_capacity_more_than_old_capacity() {
    TVector<int> vec(3);
    size_t old_capacity = vec.capacity();
    size_t new_capacity = old_capacity + 10;

    vec.reserve(new_capacity);
    bool excepted_result = true;
    bool actual_result = (vec.capacity() >= new_capacity);
    return TestSystem::check(excepted_result, actual_result);
}
bool test_16_reserve_no_change_if_new_capacity_less_than_old_capacity() {
    TVector<int> vec(4);
    size_t old_capacity = vec.capacity();

    vec.reserve(old_capacity - 1);

    bool excepted_result = true;
    bool actual_result = (vec.capacity() == old_capacity);
    return TestSystem::check(excepted_result, actual_result);
}
bool test_17_resize() {
    TVector<int> vec(3);
    vec[0] = 10;
    vec[1] = 20;
    vec[2] = 30;

    vec.resize(5); // увеличиваем размер

    const int* data = vec.data();
    const State* states = vec.states();

    size_t expected_result_size = 5;
    size_t actual_result_size = vec.size();
    return TestSystem::check(expected_result_size, actual_result_size) &&
        TestSystem::check(data[0], 10) &&
        TestSystem::check(data[2], 30) &&
        TestSystem::check(states[3], busy) &&
        TestSystem::check(states[4], busy);
}
bool test_18_is_empty() {
    TVector<int> vec;
    bool expected_result = true;
    bool actual_result = (vec.is_empty());
    return TestSystem::check(expected_result, actual_result);
}
bool test_19_push_front() {
    TVector<int> vec(3);
    vec[0] = 1;
    vec[1] = 2;
    vec[2] = 3;
    vec.push_front(0);
    size_t expected_result_size = 4;
    size_t actual_result_size = vec.size();
    size_t expected_result_capacity = 3 + vec.RESERVE_MEMORY;
    size_t actual_result_capacity = vec.capacity();
    const State* states_vec = vec.states();
    bool expected_result_state = true;
    bool actual_result_state = (states_vec[3] == State::busy);
    return TestSystem::check(expected_result_size, actual_result_size)
        && TestSystem::check(expected_result_capacity, actual_result_capacity)
        && TestSystem::check(0, vec[0])
        && TestSystem::check(3, vec[3])
        && TestSystem::check(expected_result_state, actual_result_state);
}
bool test_20_push_back() {
    TVector<int> vec(3);
    vec[0] = 1;
    vec[1] = 2;
    vec[2] = 3;
    vec.push_back(42);
    size_t expected_result_size = 4;
    size_t actual_result_size = vec.size();
    size_t expected_result_capacity = 3 + vec.RESERVE_MEMORY;
    size_t actual_result_capacity = vec.capacity();
    const State* states_vec = vec.states();
    bool expected_result_state = true;
    bool actual_result_state = (states_vec[3] == State::busy);
    return TestSystem::check(expected_result_size, actual_result_size)
        && TestSystem::check(expected_result_capacity, actual_result_capacity)
        && TestSystem::check(1, vec[0])
        && TestSystem::check(42, vec[3])
        && TestSystem::check(expected_result_state, actual_result_state);
}
bool test_21_insert() {
    TVector<int> vec(3);
    vec[0] = 1;
    vec[1] = 2;
    vec[2] = 3;
    vec.insert(1, 66);
    size_t expected_result_size = 4;
    size_t actual_result_size = vec.size();
    return TestSystem::check(expected_result_size, actual_result_size)
        && TestSystem::check(1, vec[0])
        && TestSystem::check(66, vec[1])
        && TestSystem::check(2, vec[2])
        && TestSystem::check(3, vec[3]);
}
bool test_22_pop_front() {
    TVector<int> vec(7);
    vec[0] = 10;
    vec[1] = 20;
    vec[2] = 30;
    vec[3] = 10;
    vec[4] = 20;
    vec[5] = 30;
    vec[6] = 10;
    vec.pop_front();
    const State* states_vec = vec.states();

    bool excepted_result = true;
    bool actual_result = (states_vec[0] == State::deleted && vec.size() == 7 && vec.deleted() == 1);
    return TestSystem::check(excepted_result, actual_result);
}
bool test_23_pop_back() {
    TVector<int> vec(3);
    vec[2] = 99;
    vec.pop_back();
    size_t expected_result_size = 2;
    size_t actual_result_size = vec.size();
    const State* states_vec = vec.states();
    bool excepted_result_state = true;
    bool actual_result_state = (states_vec[2] == State::empty);
    return TestSystem::check(expected_result_size, actual_result_size)
        && TestSystem::check(excepted_result_state, actual_result_state);
}
bool test_24_erase() {
    TVector<int> vec(9);
    vec.erase(1);
    const State* states_vec = vec.states();
    bool excepted_result_state = true;
    bool actual_result_state = (states_vec[1] == State::deleted);
    return TestSystem::check(excepted_result_state, actual_result_state);
}
bool test_25_emplace() {
    TVector<std::string> vec(3);
    vec.emplace(0, std::string("hello"));
    std::string expected_result = "hello";
    return TestSystem::check(expected_result, vec[0]);
}
bool test_26_pop_back_size_equal_0() {
    TVector <int> vec;
    bool expectual_res = true;
    bool actual_res = false;
    try {
        vec.pop_back();
    }
    catch (const std::exception& ex) {
        actual_res = true;
        return TestSystem::check(expectual_res, actual_res);
    }
    return TestSystem::check(expectual_res, actual_res);
}
void test_print() {
    TVector<int> vec(5);
    vec[0] = 10;
    vec[1] = 20;
    vec[2] = 30;
    vec[3] = 40;
    vec[4] = 50;
    vec.erase(2); // удалим один элемент


    vec.print();
    std::cout << std::endl;
    std::cout << std::endl;

    //std::string expected = "[ 10 20 40 50 ]";
    std::cout << "[ 10  20  40  50  ]" << std::endl;
}


bool test_27_shuffle() {
    TVector<int> vec(5);
    for (int i = 0; i < 5; ++i)
        vec[i] = i;
    TVector<int> old_vec(5);
    for (int i = 0; i < 5; ++i)
        vec[i] = i;
    shuffle(vec);
    bool expected_result = true;
    bool actual_result = false;
    for (size_t i = 0; i < 5; ++i) {
        if (vec[i] != old_vec[i]) {
            actual_result = true;
            break;
        }
    }
    return TestSystem::check(expected_result, actual_result);
}

bool test_28_quick_sort() {
    TVector<int> vec(5);
    vec[0] = 5; vec[1] = 3; vec[2] = 4; vec[3] = 1; vec[4] = 2;

    quick_sort(vec);
    bool expected_result = true;
    bool actual_result = true;
    for (int i = 0; i < 5; ++i)
        if (vec[i] != i + 1)
            actual_result = false;
    return TestSystem::check(expected_result, actual_result);
}
bool test_29_find_all() {
    TVector<int> vec(6);
    vec[0] = 1; vec[1] = 2; vec[2] = 3;
    vec[3] = 2; vec[4] = 5; vec[5] = 2;


    size_t* result = find_all(vec, 2);

    bool actual_result = (result[0] == 1 && result[1] == 3 && result[2] == 5);
    bool expected_result = true;
    delete[] result;
    return TestSystem::check(expected_result, actual_result);
}

bool test_30_find_first() {
    TVector<int> vec(5);
    vec[0] = 7;
    vec[1] = 3;
    vec[2] = 5;
    vec[3] = 3;
    vec[4] = 9;

    size_t actual_result = find_first(vec, 3);
    size_t expected_result = 1;
    return TestSystem::check(expected_result, actual_result);
}
bool test_31_find_last() {
    TVector<int> vec(5);
    vec[0] = 7;
    vec[1] = 3;
    vec[2] = 5;
    vec[3] = 3;
    vec[4] = 9;
    size_t actual_result = find_last(vec, 3);
    size_t expected_result = 3;
    return TestSystem::check(expected_result, actual_result);
}
bool test_32_push_front_with_reserve() {
    TVector<int> vec(1);
    size_t vec_capacity1 = vec.capacity();
    for (size_t i = 0; i < vec_capacity1; ++i) { // Заполнили до capacity
        vec.push_back(1);
    }
    vec.push_front(0); // должно вызвать reserve()

    bool expected_result = true;
    bool actual_result = true;
    if (vec[0] != 0) {
        actual_result = false;
    }
    if (vec_capacity1 > vec.capacity()) {
        actual_result = false;
    }
    return TestSystem::check(expected_result, actual_result);
}
bool test_33_push_back_with_reserve() {
    TVector<int> vec(1);
    size_t vec_capacity1 = vec.capacity();
    for (size_t i = 0; i < vec_capacity1; ++i) { // Заполнили до capacity
        vec.push_back(1);
    }
    vec.push_back(0); // должно вызвать reserve()

    bool expected_result = true;
    bool actual_result = true;
    if (vec[vec.size()-1] != 0) {
        actual_result = false;
    }
    if (vec_capacity1 > vec.capacity()) {
        actual_result = false;
    }
    return TestSystem::check(expected_result, actual_result);
}
bool test_34_insert_with_reserve() {
    TVector<int> vec(3);
    size_t vec_capacity1 = vec.capacity();
    for (size_t i = 0; i < vec_capacity1; ++i) { // Заполнили до capacity
        vec.insert(1, 5);
    }
    vec.insert(1, 5);
    bool expected_result = true;
    bool actual_result = true;
    if (vec[1] != 5) {
        actual_result = false;
    }
    if (vec_capacity1 > vec.capacity()) {
        actual_result = false;
    }
    return TestSystem::check(expected_result, actual_result);
}
bool test_35_insert_out_of_range() {
    TVector<int> vec(2);
    try {
        vec.insert(5, 999); // out of range
    }
    catch (const std::out_of_range&) {
        return TestSystem::check(true, true);
    }
    return TestSystem::check(true, false);
}
bool test_36_pop_front_with_shrink_to_fit() {
    TVector<int> vec(6);
    for (size_t i = 0; i < 1; ++i)
        vec.erase(i); // deleted 1
    for (size_t i = 1; i < 2; ++i)
        vec.pop_front(); // еще один deleted, должен вызваться
    size_t expected_result = 0;
    size_t actual_result = vec.deleted();
    return TestSystem::check(expected_result, actual_result);
}

bool test_37_erase_with_shrink_to_fit() {
    TVector<int> vec(20);
    for (int i = 0; i < 4; ++i)
        vec.erase(i); 
    size_t expected_result_deleted = 0;
    size_t actual_result_deleted = vec.deleted();
    size_t expected_result_size = 16;
    size_t actual_result_size = vec.size();
    return TestSystem::check(expected_result_deleted, actual_result_deleted)
        && TestSystem::check(expected_result_size, actual_result_size);
}




/*
для добавления нового теста сюда добавить функцию с данными следующего теста
в формате
bool имя_тестовой_функции() {
    1. создать тестовые данные
    2. создать правильный ответ для этих тестовых данных
    3. выполнить тестируемый метод
    4. вернуть результат сравнения ожидаемого и получившегося результатов:
    return TestSystem::check(expected_result, actual_result);
}
*/

int main() {
    test_print();

    TestSystem::start_test(test_1_default_constructor, " check default constructor");
    TestSystem::start_test(test_2_constructor_with_size, " check constructor with size");
    TestSystem::start_test(test_3_constructor_with_mass, " check constructor with mass");
    TestSystem::start_test(test_4_constructor_copy, " check constructor copy");
    TestSystem::start_test(test_5_at, " check function at");
    TestSystem::start_test(test_6_front, " check function front");
    TestSystem::start_test(test_7_back, " check function back");
    TestSystem::start_test(test_8_begin_and_end, " check function begin and end");
    TestSystem::start_test(test_9_assign, " check function assign");
    TestSystem::start_test(test_10_operator_equal, " check operator equal");
    TestSystem::start_test(test_11_clear, " check function clear");
    TestSystem::start_test(test_12_operator_equal_equal, " check operator equal equal");
    TestSystem::start_test(test_13_operator_not_equal, " check operator not equal");
    TestSystem::start_test(test_14_is_full, " check private function is_full");
    TestSystem::start_test(test_15_reserve_new_capacity_more_than_old_capacity, " check function reserve when new_capacity more than old_capacity");
    TestSystem::start_test(test_16_reserve_no_change_if_new_capacity_less_than_old_capacity, " check function reserve when no change");
    TestSystem::start_test(test_17_resize, " check function resize");
    TestSystem::start_test(test_18_is_empty, " check function is_empty");
    TestSystem::start_test(test_19_push_front, " check function push_front");
    TestSystem::start_test(test_20_push_back, " check function push_back");
    TestSystem::start_test(test_21_insert, " check function insert");
    TestSystem::start_test(test_22_pop_front, " check function pop_front");
    TestSystem::start_test(test_23_pop_back, " check function pop_back");
    TestSystem::start_test(test_24_erase, " check function erase");
    TestSystem::start_test(test_25_emplace, " check function emplace");
    TestSystem::start_test(test_26_pop_back_size_equal_0, " check function pop_back with size = 0");
    TestSystem::start_test(test_27_shuffle, " check function shuffle");
    TestSystem::start_test(test_28_quick_sort, " check function quick_sort");
    TestSystem::start_test(test_29_find_all, " check function find_all");
    TestSystem::start_test(test_30_find_first, " check function find_first");
    TestSystem::start_test(test_31_find_last, " check function find_last");
    TestSystem::start_test(test_32_push_front_with_reserve, " check function push_front_with_reserve");
    TestSystem::start_test(test_33_push_back_with_reserve, " check function push_back_with_reserve");
    TestSystem::start_test(test_34_insert_with_reserve, " check function insert_with_reserve");
    TestSystem::start_test(test_35_insert_out_of_range, " check function insert_out_of_range");
    TestSystem::start_test(test_36_pop_front_with_shrink_to_fit, " check function pop_front_with_shrink_to_fit");
    TestSystem::start_test(test_37_erase_with_shrink_to_fit, " check function erase_with_shrink_to_fit");

    TestSystem::print_final_info();
    system("pause");
    return 0;
}

#include <vector>

#include "Deque.hpp"

using namespace std;
using namespace fefu_laboratory_two;

#define CATCH_CONFIG_MAIN

#include "catch.hpp"

TEST_CASE("PUSH_BACK") {
  Deque<int> a;
  a.push_back(1);
  REQUIRE(a.back() == 1);
  a.push_back(2);
  REQUIRE(a.back() == 2);
  for (int i = 0; i < 10; ++i) {
    a.push_back(i);
  }
  REQUIRE(a.back() == 9);
}

TEST_CASE("PUSH_BACK + POP_BACK") {
  Deque<int> a;
  std::vector<int> vec{1, 3, 4, 5, 1, -12, 342, 987, 132, 98, 0, 15};
  for (auto el : vec) {
    a.push_back(el);
  }
  for (int i = 0; i < 10; ++i) {
    a.pop_back();
  }
  REQUIRE(a.back() == 3);
}

TEST_CASE("PUSH_FRONT + POP_FRONT") {
  Deque<int> a;
  std::vector<int> vec{1, 3, 4, 5, 1, -12, 342, 987, 132, 98, 0, 15};
  for (auto el : vec) {
    a.push_front(el);
  }
  for (int i = 0; i < 5; ++i) {
    a.pop_front();
  }
  REQUIRE(a.front() == 342);
}

TEST_CASE("ITERATOR PRINT DEQUE") {
  Deque<int> a;
  std::vector<int> vec{1, 3, 4, 5, 1, -12, 342, 987, 132, 98, 0, 15};
  for (auto el : vec) {
    a.push_back(el);
  }
  Deque_iterator<int> iterator;
  int index = 0;
  for (iterator = a.begin(); iterator != a.end(); iterator++) {
    REQUIRE((*iterator) == (vec[index++]));
  }
}

TEST_CASE("DEQUE[]") {
  Deque<int> a;
  std::vector<int> vec{1, 3, 4, 5, 1, -12, 342, 987, 132, 98, 0, 15};
  for (auto el : vec) {
    a.push_back(el);
  }
  REQUIRE(a[-1] == 1);
  REQUIRE(a[0] == 1);
  REQUIRE(a[1] == 3);
  REQUIRE(a[2] == 4);
  REQUIRE(a[3] == 5);
  REQUIRE(a[4] == 1);
  REQUIRE(a[5] == -12);
  REQUIRE(a[6] == 342);
  REQUIRE(a[7] == 987);
  REQUIRE(a[8] == 132);
  REQUIRE(a[9] == 98);
  REQUIRE(a[10] == 0);
  REQUIRE(a[11] == 15);
  REQUIRE(a[12] == 1);
}

TEST_CASE("ITERATOR +") {
  Deque<int> a;
  std::vector<int> vec{1, 3, 4, 5, 1, -12, 342, 987, 132, 98, 0, 15};
  for (auto el : vec) {
    a.push_back(el);
  }
  Deque<int>::iterator it = a.begin();
  REQUIRE(*(it - 1) == 1);
  REQUIRE(*(it + 0) == 1);
  REQUIRE(*(it + 1) == 3);
  REQUIRE(*(it + 2) == 4);
  REQUIRE(*(it + 3) == 5);
  REQUIRE(*(it + 4) == 1);
  REQUIRE(*(it + 5) == -12);
  REQUIRE(*(it + 6) == 342);
  REQUIRE(*(it + 7) == 987);
  REQUIRE(*(it + 8) == 132);
  REQUIRE(*(it + 9) == 98);
  REQUIRE(*(it + 10) == 0);
  REQUIRE(*(it + 11) == 15);
  REQUIRE(*(it + 12) == 15);
}

TEST_CASE("ITERATOR -") {
  Deque<int> a;
  std::vector<int> vec{1, 3, 4, 5, 1, -12, 342, 987, 132, 98, 0, 15};
  for (auto el : vec) {
    a.push_back(el);
  }
  Deque<int>::iterator it = a.begin();
  it += 11;
  REQUIRE(*(it - 1) == 0);
  REQUIRE(*(it - 2) == 98);
  REQUIRE(*(it - 3) == 132);
  REQUIRE(*(it - 4) == 987);
  REQUIRE(*(it - 5) == 342);
  REQUIRE(*(it - 6) == -12);
  REQUIRE(*(it - 7) == 1);
  REQUIRE(*(it - 8) == 5);
  REQUIRE(*(it - 9) == 4);
  REQUIRE(*(it - 10) == 3);
  REQUIRE(*(it - 11) == 1);
  REQUIRE(*(it - 12) == 1);
}

TEST_CASE("ITERATOR += -=") {
  Deque<int> a;
  std::vector<int> vec{1, 3, 4, 5, 1, -12, 342, 987, 132, 98, 0, 15};
  for (auto el : vec) {
    a.push_back(el);
  }
  Deque<int>::iterator it = a.begin();
  it += 10;
  REQUIRE(*it == 0);
  it -= 10;
  REQUIRE(*it == 1);
}

TEST_CASE("ITERATOR comparison ") {
  Deque<int> a;
  std::vector<int> vec{1, 3, 4, 5, 1, -12, 342, 987, 132, 98, 0, 15};
  for (auto el : vec) {
    a.push_back(el);
  }

  Deque<int>::iterator it = a.begin();
  Deque<int>::iterator it2 = a.begin();
  Deque<int>::iterator it3 = a.end();

  REQUIRE(it == it2);
  REQUIRE(it != it3);
  REQUIRE(it - it3 == -vec.size());
  it2++;
  REQUIRE(it2 > it);
  REQUIRE(it < it2);
  REQUIRE(it2 >= it);
  it++;
  REQUIRE(it2 >= it);
  REQUIRE(it2 <= it);
}

TEST_CASE("ITERATOR operator++(int)") {
  Deque<int> a;
  std::vector<int> vec{1, 3, 4, 5, 1, -12, 342, 987, 132, 98, 0, 15};
  for (auto el : vec) {
    a.push_back(el);
  }
  Deque<int>::iterator it = a.begin();
  Deque<int>::iterator it2 = a.begin();
  REQUIRE(it == it2);
  REQUIRE(it == it2++);
  REQUIRE(it2 > it);
  REQUIRE(*it == 1);
  REQUIRE(*it2 == 3);
}

TEST_CASE("ITERATOR operator --(int)") {
  Deque<int> a;
  std::vector<int> vec{1, 3, 4, 5, 1, -12, 342, 987, 132, 98, 0, 15};
  for (auto el : vec) {
    a.push_back(el);
  }
  Deque<int>::iterator it = a.begin();
  Deque<int>::iterator it2 = a.begin();
  REQUIRE(it == it2);
  it += 5;
  it2 += 5;
  REQUIRE(*it == -12);
  REQUIRE(*it2 == -12);
  REQUIRE(it == it2--);
}

TEST_CASE("DEQUE comparison") {
  Deque<int> a;
  std::vector<int> vec{1, 3, 4, 5, 1, -12, 342, 987, 132, 98, 0, 15};
  for (auto el : vec) {
    a.push_back(el);
  }

  Deque<int> b;
  std::vector<int> vec2{1, 3, 4, 5, 1, -12, 342, 987, 132, 98, 0, 15};
  for (auto el : vec2) {
    b.push_back(el);
  }
  REQUIRE(a == b);
  REQUIRE(a <= b);
  REQUIRE(a >= b);

  {
    Deque<int> a1;
    std::vector<int> vec1{1, 3, 4, 5, 1, -12, 342, 987, 132, 98, 0, 15};
    for (auto el : vec1) {
      a1.push_back(el);
    }

    Deque<int> b1;
    std::vector<int> vec12{1, 3, 4, 5, 1, -12, 342, 987, 132, 98, 0, 14};
    for (auto el : vec12) {
      b1.push_back(el);
    }

    REQUIRE(b1 < a1);
    REQUIRE(b1 <= a1);
    REQUIRE(a1 > b1);
    REQUIRE(a1 >= b1);
    REQUIRE(a1 != b1);
  }

  {
    Deque<int> a2;
    std::vector<int> vec2{1, 1, 1, 1};
    for (auto el : vec2) {
      a2.push_back(el);
    }

    Deque<int> b2;
    std::vector<int> vec22{1, 1, 1};
    for (auto el : vec22) {
      b2.push_back(el);
    }

    REQUIRE(b2 <= a2);
    REQUIRE(b2 < a2);
    REQUIRE(a2 >= b2);
    REQUIRE(a2 > b2);
    REQUIRE(a2 != b2);
  }
}

TEST_CASE("DEQUE size()") {
  Deque<int> a;
  std::vector<int> vec{1, 3, 4, 5, 1, -12, 342, 987, 132, 98, 0, 15};
  for (auto el : vec) {
    a.push_back(el);
  }
  REQUIRE(a.size() == 12);
  a.pop_back();
  REQUIRE(a.size() == 11);
  for (int i = 0; i < 6; ++i) {
    a.pop_back();
  }
  REQUIRE(a.size() == 5);
  for (int i = 0; i < 100; ++i) {
    a.push_back(i);
    a.push_front(i);
  }
  REQUIRE(a.size() == 205);
}

TEST_CASE("DEQUE at()") {
  Deque<int> a;
  std::vector<int> vec{1, 3, 4, 5, 1, -12, 342, 987, 132, 98, 0, 15};
  for (auto el : vec) {
    a.push_back(el);
  }
  REQUIRE(a.at(0) == 1);
  REQUIRE(a.at(1) == 3);
  REQUIRE(a.at(2) == 4);
  REQUIRE(a.at(3) == 5);
  REQUIRE(a.at(4) == 1);
  REQUIRE(a.at(5) == -12);
  REQUIRE(a.at(6) == 342);
  REQUIRE(a.at(7) == 987);
  REQUIRE(a.at(8) == 132);
  REQUIRE(a.at(9) == 98);
  REQUIRE(a.at(10) == 0);
  REQUIRE(a.at(11) == 15);
}

TEST_CASE("DEQUE back()") {
  Deque<int> a;
  std::vector<int> vec{1, 3, 4, 5, 1, -12, 342, 987, 132, 98, 0, 15};
  for (auto el : vec) {
    a.push_back(el);
  }
  REQUIRE(a.back() == 15);
  a.pop_back();
  REQUIRE(a.back() == 0);
  for (int i = 0; i < 5; ++i) {
    a.pop_back();
  }
  REQUIRE(a.back() == -12);
}

TEST_CASE("DEQUE front()") {
  Deque<int> a;
  std::vector<int> vec{1, 3, 4, 5, 1, -12, 342, 987, 132, 98, 0, 15};
  for (auto el : vec) {
    a.push_back(el);
  }
  REQUIRE(a.front() == 1);
  a.pop_back();
  a.push_front(5);
  REQUIRE(a.front() == 5);
}

TEST_CASE("reverse_iterator PRINT") {
  Deque<int> a;
  std::vector<int> vec{1, 3, 4, 5, 1, -12, 342, 987, 132, 98, 0, 15};
  for (auto el : vec) {
    a.push_back(el);
  }
  Deque<int>::reverse_iterator rev = a.rbegin();
  REQUIRE(*rev == 15);
  REQUIRE(*(++rev) == 0);
  REQUIRE(*(++rev) == 98);
  REQUIRE(*(++rev) == 132);
  REQUIRE(*(++rev) == 987);
  REQUIRE(*(++rev) == 342);
  REQUIRE(*(++rev) == -12);
  REQUIRE(*(++rev) == 1);
  REQUIRE(*(++rev) == 5);
  REQUIRE(*(++rev) == 4);
  REQUIRE(*(++rev) == 3);
  REQUIRE(*(++rev) == 1);
  REQUIRE(*(++rev) == 1);
}

TEST_CASE("DEQUE resize()") {
  Deque<int> a;
  std::vector<int> vec{1, 3, 4, 5, 1, -12, 342, 987, 132, 98, 0, 15};
  for (auto el : vec) {
    a.push_back(el);
  }
  REQUIRE(a.size() == 12);
  size_t count = 5;
  a.resize(count);
  REQUIRE(a.size() == 5);
}

TEST_CASE("DEQUE resize(count, value)") {
  Deque<int> a;
  std::vector<int> vec{1, 3, 4, 5, 1, -12, 342, 987, 132, 98, 0, 15};
  for (auto el : vec) {
    a.push_back(el);
  }
  REQUIRE(a.size() == 12);
  size_t count = 20;
  int value = 0;
  a.resize(count, value);
  REQUIRE(a.size() == count);
  for (int i = 0; i < count - 1; ++i) {
    REQUIRE(a[i] == 0);
  }
}

TEST_CASE("DEQUE resize(count, value) 2") {
  Deque<int> a;
  std::vector<int> vec{1, 3, 4, 5, 1, -12, 342, 987, 132, 98, 0, 15};
  for (auto el : vec) {
    a.push_back(el);
  }
  REQUIRE(a.size() == 12);
  size_t count = 5;
  int value = 123456789;
  a.resize(count, value);
  REQUIRE(a.size() == count);
  for (int i = 0; i < count - 1; ++i) {
    REQUIRE(a[i] == 123456789);
  }
}

TEST_CASE("DEQUE clear()") {
  Deque<int> b;
  std::vector<int> vec2{1, 3, 4, 5, 1, -12, 342, 987, 132, 98, 0, 15};
  for (auto el : vec2) {
    b.push_back(el);
  }
  b.clear();
  REQUIRE(b.size() == 0);
}

TEST_CASE("DEQUE empty") {
  Deque<int> a;
  std::vector<int> vec{1, 3, 4, 5, 1, -12, 342, 987, 132, 98, 0, 15};
  for (auto el : vec) {
    a.push_back(el);
  }
  REQUIRE(a.empty() == false);
  a.clear();
  REQUIRE(a.empty());
}

TEST_CASE("DEQUE assign") {
  Deque<int> a;
  std::vector<int> vec{1, 3, 4, 5, 1, -12, 342, 987, 132, 98, 0, 15};
  for (auto el : vec) {
    a.push_back(el);
  }
  size_t count = 5;
  int value = 10;
  a.assign(count, value);
  REQUIRE(a.size() == count);
  for (int i = 0; i < count - 1; ++i) {
    REQUIRE(a.at(i) == 10);
  }
}

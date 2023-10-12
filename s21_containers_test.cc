#include "s21_containers.h"

#include <gtest/gtest.h>

#include <array>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <vector>

#include "s21_containersplus.h"

// array tests

class tester_class {
 public:
  tester_class() {
    m = new int[10];
    for (int i = 0; i < 10; i++) m[i] = i;
  }

  tester_class(int *_m, int _n) : m(_m), n(_n) {}

  tester_class(const tester_class &other) {
    m = new int[10];
    for (int i = 0; i < 10; i++) m[i] = other.m[i];
  }

  tester_class(tester_class &&other) noexcept {
    m = other.m;
    other.m = nullptr;
  }

  tester_class &operator=(const tester_class &other) {
    for (int i = 0; i < 10; i++) m[i] = other.m[i];
    return *this;
  }

  tester_class &operator=(tester_class &&other) {
    delete[] m;
    m = other.m;
    other.m = nullptr;
    return *this;
  }

  bool operator!=(const tester_class &other) const { return other.n != 10; }

  bool operator==(const tester_class &other) const { return other.n == 10; }

  ~tester_class() { delete[] m; }

  int *m;
  int n = 10;
};

class throw_tester_class {
 public:
  throw_tester_class() {
    m = new int[10];
    for (int i = 0; i < 10; i++) m[i] = i;
  }
  throw_tester_class(const throw_tester_class &other) {
    if (other.n == 1) {
      throw std::out_of_range("qwe");
    }
    m = new int[10];
    for (int i = 0; i < 10; i++) m[i] = other.m[i];
    n = other.n;
  }
  throw_tester_class(throw_tester_class &&other) {
    m = other.m;
    n = other.n;
    other.n = 0;
    other.m = nullptr;
  }
  throw_tester_class &operator=(const throw_tester_class &other) {
    if (n == 1) {
      delete[] m;
      throw;
    }
    for (int i = 0; i < 10; i++) m[i] = other.m[i];

    n = other.n;
    return *this;
  }
  throw_tester_class &operator=(throw_tester_class &&other) {
    delete[] m;
    n = other.n;
    m = other.m;
    other.m = nullptr;
    return *this;
  }

  bool operator!=(const throw_tester_class &other) const {
    return other.n != 10;
  }

  ~throw_tester_class() {
    if (m != nullptr) delete[] m;
  }

  int *m = nullptr;
  int n = 10;
};

template <class T, class Allocator>
bool compare_to_std(const std::vector<T, Allocator> &std_vec,
                    const s21::vector<T, Allocator> &s21_vec,
                    bool compare_elements) {
  if (std_vec.size() == s21_vec.size() &&
      std_vec.capacity() == s21_vec.capacity() &&
      std_vec.empty() == s21_vec.empty() &&
      std_vec.max_size() == s21_vec.max_size() &&
      std_vec.get_allocator() == s21_vec.get_allocator()) {
    if (compare_elements)
      for (size_t i = 0; i < std_vec.size(); i++)
        if (std_vec[i] != s21_vec[i]) return false;
    return true;
  }
  return false;
}

template <class T, std::size_t N>
bool compare_to_std(const std::array<T, N> &std_arr,
                    const s21::array<T, N> &s21_arr, bool compare_elements) {
  if (std_arr.size() == s21_arr.size() && std_arr.empty() == s21_arr.empty() &&
      std_arr.max_size() == s21_arr.max_size() &&
      std_arr.front() == s21_arr.front() && std_arr.back() == s21_arr.back()) {
    if (compare_elements)
      for (size_t i = 0; i < s21_arr.size(); i++)
        if (std_arr[i] != s21_arr[i]) return false;
    return true;
  }
  return false;
}

TEST(array, init) {
  std::array<double, 5> stdarr1 = {1, 2, 3, 4, 5};
  s21::array<double, 5> s21arr1 = {1, 2, 3, 4, 5};
  EXPECT_EQ(compare_to_std(stdarr1, s21arr1, true), true);

  std::array<tester_class, 5> stdarr2 = {tester_class()};
  s21::array<tester_class, 5> s21arr2 = {tester_class()};
  EXPECT_EQ(compare_to_std(stdarr2, s21arr2, false), true);

  std::array<double, 5> stdarr3({});
  s21::array<double, 5> s21arr3({});
  EXPECT_EQ(compare_to_std(stdarr3, s21arr3, true), true);
}

TEST(array, iterators) {
  int a = 0;
  std::array<int, 10> stdarr1 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
  for (auto it = stdarr1.begin(); it != stdarr1.cend(); ++it) *it = a++;
  a = 0;
  s21::array<int, 10> s21arr1 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
  for (auto it = s21arr1.begin(); it != s21arr1.cend(); ++it) *it = a++;
  EXPECT_EQ(compare_to_std(stdarr1, s21arr1, true), true);

  std::array<int, 10>::const_iterator stditer1 = stdarr1.begin() + 1;
  s21::array<int, 10>::const_iterator s21iter1 = s21arr1.begin() + 1;
  EXPECT_EQ(*stditer1, *s21iter1);

  stditer1++;
  s21iter1++;
  EXPECT_EQ(*stditer1, *s21iter1);

  EXPECT_EQ(*++stditer1, *++s21iter1);

  EXPECT_EQ(*stditer1--, *s21iter1--);

  --stditer1;
  --s21iter1;

  EXPECT_EQ(*(stditer1 + 2), *(s21iter1 + 2));

  EXPECT_EQ(*(stditer1 - 1), *(s21iter1 - 1));

  EXPECT_EQ(stditer1[2], s21iter1[2]);

  EXPECT_EQ(*(stditer1 += 3), *(s21iter1 += 3));

  EXPECT_EQ(*(stditer1 -= 2), *(s21iter1 -= 2));

  EXPECT_EQ(s21iter1 - s21arr1.begin(), stditer1 - stdarr1.begin());

  std::array<int, 10>::iterator stditer2(stdarr1.data());
  s21::array<int, 10>::iterator s21iter2(s21arr1.data());

  EXPECT_EQ(stditer2 == stdarr1.cbegin(), s21iter2 == s21arr1.cbegin());

  EXPECT_EQ(stditer2 != stdarr1.cend(), s21iter2 != s21arr1.cend());

  EXPECT_EQ(stditer2 <= stdarr1.cend(), s21iter2 <= s21arr1.cend());
  EXPECT_EQ(stditer2 <= stdarr1.cbegin(), s21iter2 <= s21arr1.cbegin());

  EXPECT_EQ(stditer2 < stdarr1.cend(), s21iter2 < s21arr1.cend());
  EXPECT_EQ(stditer2 < stdarr1.cbegin(), s21iter2 < s21arr1.cbegin());

  EXPECT_EQ(stditer2 >= stdarr1.cend(), s21iter2 >= s21arr1.cend());
  EXPECT_EQ(stditer2 >= stdarr1.cbegin(), s21iter2 >= s21arr1.cbegin());

  EXPECT_EQ(stditer2 > stdarr1.cend(), s21iter2 > s21arr1.cend());
  EXPECT_EQ(stditer2 > stdarr1.cbegin(), s21iter2 > s21arr1.cbegin());

  std::array<tester_class, 1> stdarr2({tester_class()});
  s21::array<tester_class, 1> s21arr2({tester_class()});

  std::array<int, 10>::reverse_iterator stdreviter1 = stdarr1.rbegin();
  std::array<int, 10>::const_reverse_iterator stdreviter2 = stdarr1.rbegin();
  std::array<int, 10>::const_reverse_iterator stdreviter3 = stdarr1.crbegin();

  s21::array<int, 10>::reverse_iterator s21reviter1 = s21arr1.rbegin();
  s21::array<int, 10>::const_reverse_iterator s21reviter2 = s21arr1.rbegin();
  s21::array<int, 10>::const_reverse_iterator s21reviter3 = s21arr1.crbegin();

  EXPECT_EQ(*stdreviter1, *s21reviter1);
  for (; s21reviter2 != s21arr1.rend(); ++stdreviter2, ++s21reviter2)
    EXPECT_EQ(*stdreviter2, *s21reviter2);

  EXPECT_EQ(*stdreviter3, *s21reviter3);

  EXPECT_EQ(stdarr2.begin()->n, s21arr2.begin()->n);

  const std::array<int, 10>::const_reverse_iterator stdreviter4 =
      stdarr1.crend() - 1;
  const s21::array<int, 10>::const_reverse_iterator s21reviter4 =
      s21arr1.crend() - 1;
  EXPECT_EQ(*stdreviter4, *s21reviter4);
}

TEST(array, accessers) {
  std::array<int, 10> stdarr1({1, 2, 3, 4, 5, 6, 7, 8, 9, 0});
  s21::array<int, 10> s21arr1({1, 2, 3, 4, 5, 6, 7, 8, 9, 0});

  for (int i = 0; i < 10; ++i) {
    EXPECT_EQ(stdarr1.at(i), s21arr1.at(i));
    EXPECT_EQ(stdarr1[i], s21arr1[i]);
  }

  EXPECT_EQ(stdarr1.front(), s21arr1.front());

  EXPECT_EQ(stdarr1.back(), s21arr1.back());

  EXPECT_EQ(*(stdarr1.data() + 2), *(s21arr1.data() + 2));
}

TEST(array, modifiers) {
  std::array<int, 5> stdarr1({1, 2, 3});
  s21::array<int, 5> s21arr1({1, 2, 3});

  EXPECT_EQ(compare_to_std(stdarr1, s21arr1, true), true);

  std::array<int, 5> stdarr2({0, 1, 2, 3, 4});
  s21::array<int, 5> s21arr2({0, 1, 2, 3, 4});

  EXPECT_EQ(compare_to_std(stdarr2, s21arr2, true), true);

  stdarr1.swap(stdarr2);
  s21arr1.swap(s21arr2);

  EXPECT_EQ(compare_to_std(stdarr1, s21arr1, true), true);
  EXPECT_EQ(compare_to_std(stdarr2, s21arr2, true), true);

  stdarr1.fill(100);
  s21arr1.fill(100);

  EXPECT_EQ(compare_to_std(stdarr1, s21arr1, true), true);
}

// vector tests

TEST(vector, constructors) {
  std::vector<double> stdvec_inner(50, 5.0);
  s21::vector<double> s21vec_inner(50, 5.0);

  std::vector<std::vector<double>> stdvec1;
  s21::vector<std::vector<double>> s21vec1;
  EXPECT_EQ(compare_to_std(stdvec1, s21vec1, false), true);

  s21::vector<int> s21vec2(std::allocator<int>);

  std::vector<std::vector<double>> stdvec3(10, stdvec_inner);
  s21::vector<std::vector<double>> s21vec3(10, stdvec_inner);
  EXPECT_EQ(compare_to_std(stdvec3, s21vec3, false), true);
  for (int i = 0; i < 10; ++i)
    for (int j = 0; j < 50; ++j) EXPECT_EQ(s21vec3[i][j], 5.0);

  std::vector<double> stdvec4(stdvec_inner.begin(), stdvec_inner.end());
  s21::vector<double> s21vec4(stdvec_inner.begin(), stdvec_inner.end());
  EXPECT_EQ(compare_to_std(stdvec4, s21vec4, true), true);

  std::vector<std::vector<double>> stdvec5(stdvec3);
  s21::vector<std::vector<double>> s21vec5(s21vec3);

  std::vector<double> stdvec6(stdvec_inner);
  s21::vector<double> s21vec6(s21vec_inner);
  EXPECT_EQ(compare_to_std(stdvec6, s21vec6, true), true);

  std::vector<double> stdvec8(std::move(stdvec_inner));
  s21::vector<double> s21vec8(std::move(s21vec_inner));
  EXPECT_EQ(compare_to_std(stdvec8, s21vec8, true), true);
  EXPECT_EQ(compare_to_std(stdvec_inner, s21vec_inner, true), true);

  std::vector<double> stdvec10({1, 2, 3, 4, 5, 6, 7});
  s21::vector<double> s21vec10({1, 2, 3, 4, 5, 6, 7});

  EXPECT_EQ(compare_to_std(stdvec10, s21vec10, true), true);
}

TEST(vector, assignments) {
  std::vector<double> stdvec_inner({1, 2, 3, 4, 5, 6, 7});
  s21::vector<double> s21vec_inner({1, 2, 3, 4, 5, 6, 7});

  std::vector<tester_class> stdvec1(3);
  s21::vector<tester_class> s21vec1(3);

  std::vector<tester_class> stdvec2(4);
  stdvec2 = stdvec1;

  s21::vector<tester_class> s21vec2(4);
  s21vec2 = s21vec1;

  EXPECT_EQ(compare_to_std(stdvec2, s21vec2, false), true);

  std::vector<tester_class> stdvec3;
  s21::vector<tester_class> s21vec3;

  stdvec3 = std::move(stdvec2);
  s21vec3 = std::move(s21vec2);

  EXPECT_EQ(compare_to_std(stdvec3, s21vec3, false), true);

  std::vector<double> stdvec4;
  s21::vector<double> s21vec4;

  stdvec4 = {1, 2, 3, 4, 5};
  s21vec4 = {1, 2, 3, 4, 5};

  EXPECT_EQ(compare_to_std(stdvec4, s21vec4, true), true);

  stdvec4.assign(15, 20.5);
  s21vec4.assign(15, 20.5);

  EXPECT_EQ(compare_to_std(stdvec4, s21vec4, true), true);

  stdvec4.assign(3, 20.5);
  s21vec4.assign(3, 20.5);

  EXPECT_EQ(compare_to_std(stdvec4, s21vec4, true), true);

  stdvec4.assign(stdvec_inner.begin(), stdvec_inner.end());
  s21vec4.assign(s21vec_inner.begin(), s21vec_inner.end());

  EXPECT_EQ(compare_to_std(stdvec4, s21vec4, true), true);

  stdvec4.assign({0, 2, 0, 4});
  s21vec4.assign({0, 2, 0, 4});

  EXPECT_EQ(compare_to_std(stdvec4, s21vec4, true), true);
}

TEST(vector, accessers) {
  std::vector<int> stdvec1({1, 2, 3, 4, 5, 6, 7, 8, 9, 0});
  s21::vector<int> s21vec1({1, 2, 3, 4, 5, 6, 7, 8, 9, 0});

  EXPECT_EQ(stdvec1.get_allocator(), s21vec1.get_allocator());
  for (int i = 0; i < 10; ++i) {
    EXPECT_EQ(stdvec1.at(i), s21vec1.at(i));
    EXPECT_EQ(stdvec1[i], s21vec1[i]);
  }

  EXPECT_EQ(stdvec1.front(), s21vec1.front());

  EXPECT_EQ(stdvec1.back(), s21vec1.back());

  EXPECT_EQ(*(stdvec1.data() + 2), *(s21vec1.data() + 2));
}

TEST(vector, iterators) {
  int a = 0;
  std::vector<int> stdvec1(10);
  for (auto it = stdvec1.begin(); it != stdvec1.cend(); ++it) *it = a++;
  a = 0;
  s21::vector<int> s21vec1(10);
  for (auto it = s21vec1.begin(); it != s21vec1.cend(); ++it) *it = a++;
  EXPECT_EQ(compare_to_std(stdvec1, s21vec1, true), true);

  std::vector<int>::const_iterator stditer1 = stdvec1.begin() + 1;
  s21::vector<int>::const_iterator s21iter1 = s21vec1.begin() + 1;
  EXPECT_EQ(*stditer1, *s21iter1);

  stditer1++;
  s21iter1++;
  EXPECT_EQ(*stditer1, *s21iter1);

  EXPECT_EQ(*++stditer1, *++s21iter1);

  EXPECT_EQ(*stditer1--, *s21iter1--);

  --stditer1;
  --s21iter1;

  EXPECT_EQ(*(stditer1 + 2), *(s21iter1 + 2));

  EXPECT_EQ(*(stditer1 - 1), *(s21iter1 - 1));

  EXPECT_EQ(stditer1[2], s21iter1[2]);

  EXPECT_EQ(*(stditer1 += 3), *(s21iter1 += 3));

  EXPECT_EQ(*(stditer1 -= 2), *(s21iter1 -= 2));

  EXPECT_EQ(s21iter1 - s21vec1.begin(), stditer1 - stdvec1.begin());

  std::vector<int>::iterator stditer2{stdvec1.begin()};
  s21::vector<int>::iterator s21iter2{s21vec1.begin()};

  EXPECT_EQ(stditer2 == stdvec1.cbegin(), s21iter2 == s21vec1.cbegin());

  EXPECT_EQ(stditer2 != stdvec1.cend(), s21iter2 != s21vec1.cend());

  EXPECT_EQ(stditer2 <= stdvec1.cend(), s21iter2 <= s21vec1.cend());
  EXPECT_EQ(stditer2 <= stdvec1.cbegin(), s21iter2 <= s21vec1.cbegin());

  EXPECT_EQ(stditer2 < stdvec1.cend(), s21iter2 < s21vec1.cend());
  EXPECT_EQ(stditer2 < stdvec1.cbegin(), s21iter2 < s21vec1.cbegin());

  EXPECT_EQ(stditer2 >= stdvec1.cend(), s21iter2 >= s21vec1.cend());
  EXPECT_EQ(stditer2 >= stdvec1.cbegin(), s21iter2 >= s21vec1.cbegin());

  EXPECT_EQ(stditer2 > stdvec1.cend(), s21iter2 > s21vec1.cend());
  EXPECT_EQ(stditer2 > stdvec1.cbegin(), s21iter2 > s21vec1.cbegin());

  std::vector<tester_class> stdvec2(1);
  s21::vector<tester_class> s21vec2(1);

  std::vector<int>::reverse_iterator stdreviter1 = stdvec1.rbegin();
  std::vector<int>::const_reverse_iterator stdreviter2 = stdvec1.rbegin();
  std::vector<int>::const_reverse_iterator stdreviter3 = stdvec1.crbegin();

  s21::vector<int>::reverse_iterator s21reviter1 = s21vec1.rbegin();
  s21::vector<int>::const_reverse_iterator s21reviter2 = s21vec1.rbegin();
  s21::vector<int>::const_reverse_iterator s21reviter3 = s21vec1.crbegin();

  EXPECT_EQ(*stdreviter1, *s21reviter1);
  for (; s21reviter2 != s21vec1.rend(); ++stdreviter2, ++s21reviter2)
    EXPECT_EQ(*stdreviter2, *s21reviter2);

  EXPECT_EQ(*stdreviter3, *s21reviter3);

  EXPECT_EQ(stdvec2.begin()->n, s21vec2.begin()->n);

  s21::vector<int>::iterator s21iter0;

  const std::vector<int>::const_reverse_iterator stdreviter4 =
      stdvec1.crend() - 1;
  const s21::vector<int>::const_reverse_iterator s21reviter4 =
      s21vec1.crend() - 1;
  EXPECT_EQ(*stdreviter4, *s21reviter4);
}

TEST(vector, capacity) {
  std::vector<int> stdvec_inner(100);

  std::vector<std::vector<int>> stdvec1(100, stdvec_inner);
  s21::vector<std::vector<int>> s21vec1(100, stdvec_inner);
  EXPECT_EQ(compare_to_std(stdvec1, s21vec1, false), true);

  stdvec1.reserve(100);
  s21vec1.reserve(100);
  EXPECT_EQ(compare_to_std(stdvec1, s21vec1, false), true);

  stdvec1.reserve(120);
  s21vec1.reserve(120);
  EXPECT_EQ(compare_to_std(stdvec1, s21vec1, false), true);

  stdvec1.shrink_to_fit();
  s21vec1.shrink_to_fit();
  EXPECT_EQ(compare_to_std(stdvec1, s21vec1, false), true);

  stdvec1.shrink_to_fit();
  s21vec1.shrink_to_fit();
  EXPECT_EQ(compare_to_std(stdvec1, s21vec1, false), true);

  stdvec1.reserve(1);
  s21vec1.reserve(1);
  EXPECT_EQ(compare_to_std(stdvec1, s21vec1, false), true);

  stdvec1.clear();
  s21vec1.clear();
  EXPECT_EQ(compare_to_std(stdvec1, s21vec1, false), true);
}

TEST(vector, modifiers) {
  std::vector<tester_class> stdvec1(10);
  s21::vector<tester_class> s21vec1(10);
  EXPECT_EQ(compare_to_std(stdvec1, s21vec1, false), true);

  stdvec1.clear();
  s21vec1.clear();
  EXPECT_EQ(compare_to_std(stdvec1, s21vec1, false), true);

  stdvec1.clear();
  s21vec1.clear();
  EXPECT_EQ(compare_to_std(stdvec1, s21vec1, false), true);

  stdvec1.insert(stdvec1.end(), tester_class());
  s21vec1.insert(s21vec1.end(), tester_class());
  EXPECT_EQ(compare_to_std(stdvec1, s21vec1, false), true);

  std::vector<double> stdvec2(10);
  s21::vector<double> s21vec2(10);

  stdvec2.insert(stdvec2.begin() + 2, 3.4);
  s21vec2.insert(s21vec2.begin() + 2, 3.4);

  EXPECT_EQ(compare_to_std(stdvec2, s21vec2, true), true);

  stdvec2.insert(stdvec2.begin() + 2, 3.4);
  s21vec2.insert(s21vec2.begin() + 2, 3.4);

  EXPECT_EQ(compare_to_std(stdvec2, s21vec2, true), true);

  stdvec2.insert(stdvec2.begin() + 2, 100, 3.4);
  s21vec2.insert(s21vec2.begin() + 2, 100, 3.4);

  EXPECT_EQ(compare_to_std(stdvec2, s21vec2, true), true);

  stdvec2.insert(stdvec2.begin() + 2, {1, 2, 3, 4, 5});
  s21vec2.insert(s21vec2.begin() + 2, {1, 2, 3, 4, 5});

  EXPECT_EQ(compare_to_std(stdvec2, s21vec2, true), true);

  stdvec2.insert(stdvec2.begin() + 10, s21vec2.begin(), s21vec2.begin() + 2);
  s21vec2.insert(s21vec2.begin() + 10, stdvec2.begin(), stdvec2.begin() + 2);

  EXPECT_EQ(compare_to_std(stdvec2, s21vec2, true), true);

  tester_class tester_class1;

  std::vector<tester_class> stdvec3;
  s21::vector<tester_class> s21vec3;

  stdvec3.insert(stdvec3.end(), tester_class1);
  s21vec3.insert(s21vec3.end(), tester_class1);

  stdvec3.insert(stdvec3.end(), tester_class1);
  s21vec3.insert(s21vec3.end(), tester_class1);

  EXPECT_EQ(compare_to_std(stdvec3, s21vec3, false), true);

  stdvec3.reserve(10);
  s21vec3.reserve(10);

  stdvec3.insert(stdvec3.end(), tester_class1);
  s21vec3.insert(s21vec3.end(), tester_class1);

  EXPECT_EQ(compare_to_std(stdvec3, s21vec3, false), true);

  stdvec3.insert(stdvec3.end(), 3, tester_class1);
  s21vec3.insert(s21vec3.end(), 3, tester_class1);

  EXPECT_EQ(compare_to_std(stdvec3, s21vec3, false), true);

  stdvec3.shrink_to_fit();
  s21vec3.shrink_to_fit();

  stdvec3.insert(stdvec3.end(), s21vec3.begin() + 1, s21vec3.begin() + 2);
  s21vec3.insert(s21vec3.end(), stdvec3.begin() + 1, stdvec3.begin() + 2);

  EXPECT_EQ(compare_to_std(stdvec3, s21vec3, false), true);

  stdvec3.insert(stdvec3.end(), {tester_class1});
  s21vec3.insert(s21vec3.end(), {tester_class1});

  EXPECT_EQ(compare_to_std(stdvec3, s21vec3, false), true);

  stdvec2.erase(stdvec2.begin() + 3);
  s21vec2.erase(s21vec2.begin() + 3);

  EXPECT_EQ(compare_to_std(stdvec2, s21vec2, true), true);

  stdvec2.erase(stdvec2.begin() + 3, stdvec2.end());
  s21vec2.erase(s21vec2.begin() + 3, s21vec2.end());

  EXPECT_EQ(compare_to_std(stdvec2, s21vec2, true), true);

  stdvec3.erase(stdvec3.begin(), stdvec3.begin() + 2);
  s21vec3.erase(s21vec3.begin(), s21vec3.begin() + 2);

  EXPECT_EQ(compare_to_std(stdvec3, s21vec3, false), true);

  stdvec3.clear();
  s21vec3.clear();

  stdvec3.shrink_to_fit();
  s21vec3.shrink_to_fit();

  stdvec3.push_back(tester_class1);
  s21vec3.push_back(tester_class1);

  EXPECT_EQ(compare_to_std(stdvec3, s21vec3, false), true);

  stdvec3.push_back(tester_class1);
  s21vec3.push_back(tester_class1);

  EXPECT_EQ(compare_to_std(stdvec3, s21vec3, false), true);

  stdvec2.clear();
  s21vec2.clear();

  stdvec2.shrink_to_fit();
  s21vec2.shrink_to_fit();

  stdvec2.push_back(100);
  s21vec2.push_back(100);

  EXPECT_EQ(compare_to_std(stdvec2, s21vec2, true), true);

  stdvec2.push_back(100);
  s21vec2.push_back(100);

  EXPECT_EQ(compare_to_std(stdvec2, s21vec2, true), true);

  stdvec2.pop_back();
  s21vec2.pop_back();

  EXPECT_EQ(compare_to_std(stdvec2, s21vec2, true), true);

  stdvec2.resize(100);
  s21vec2.resize(100);

  EXPECT_EQ(compare_to_std(stdvec2, s21vec2, true), true);

  stdvec2.resize(2);
  s21vec2.resize(2);
  EXPECT_EQ(compare_to_std(stdvec2, s21vec2, true), true);

  stdvec2.resize(3, 10);
  s21vec2.resize(3, 10);

  EXPECT_EQ(compare_to_std(stdvec2, s21vec2, true), true);

  stdvec2.resize(1, 10);
  s21vec2.resize(1, 10);

  EXPECT_EQ(compare_to_std(stdvec2, s21vec2, true), true);

  std::vector<tester_class> stdvec4(15);
  std::vector<tester_class> stdvec5(4);
  stdvec4.reserve(100);

  s21::vector<tester_class> s21vec4(15);
  s21::vector<tester_class> s21vec5(4);
  s21vec4.reserve(100);

  s21vec4.swap(s21vec5);
  s21vec4.swap(s21vec5);

  EXPECT_EQ(compare_to_std(stdvec4, s21vec4, false), true);
  EXPECT_EQ(compare_to_std(stdvec5, s21vec5, false), true);

  int *m1 = new int[10];
  int *m2 = new int[10];

  s21vec3.emplace_back(m1, 12);
  stdvec3.emplace_back(m2, 12);

  s21vec3.emplace_back(tester_class());
  stdvec3.emplace_back(tester_class());

  int *m3 = new int[10];
  int *m4 = new int[10];

  s21vec3.emplace(s21vec3.begin() + 1, m3, 10);
  stdvec3.emplace(stdvec3.begin() + 1, m4, 10);

  EXPECT_EQ(compare_to_std(stdvec3, s21vec3, false), true);
  EXPECT_EQ(s21vec3.back().n, stdvec3.back().n);
}

TEST(vector, throws) {
  bool catched = false;
  s21::vector<int> s21vec1(100);
  try {
    s21vec1.reserve(s21vec1.max_size() + 1);
  } catch (...) {
    catched = true;
  }
  EXPECT_EQ(catched, true);
  catched = false;

  try {
    s21vec1.at(101) = 10;
  } catch (...) {
    catched = true;
  }
  EXPECT_EQ(catched, true);
  catched = false;

  s21::vector<throw_tester_class> s21vec2(15);
  s21::vector<throw_tester_class> s21vec3(15);

  std::vector<throw_tester_class> stdvec2(15);
  std::vector<throw_tester_class> stdvec3(15);

  s21vec3[1].n = 1;
  stdvec3[1].n = 1;

  throw_tester_class tester_class1;
  throw_tester_class tester_class2;
  throw_tester_class tester_class3;
  tester_class1.n = 1;
  tester_class2.n = 1;
  tester_class3.n = 1;
  try {
    s21vec2.push_back(tester_class1);
  } catch (...) {
    catched = true;
  }
  EXPECT_EQ(catched, true);
  catched = false;

  try {
    stdvec2.push_back(tester_class1);
  } catch (...) {
    catched = true;
  }
  EXPECT_EQ(catched, true);
  EXPECT_EQ(compare_to_std(stdvec2, s21vec2, false), true);
  catched = false;

  try {
    s21vec2.insert(s21vec2.begin() + 2, tester_class1);
  } catch (...) {
    catched = true;
  }
  EXPECT_EQ(catched, true);
  catched = false;

  try {
    stdvec2.insert(stdvec2.begin() + 2, tester_class1);
  } catch (...) {
    catched = true;
  }
  EXPECT_EQ(catched, true);
  EXPECT_EQ(compare_to_std(stdvec2, s21vec2, false), true);
  catched = false;

  try {
    s21vec2.insert(s21vec2.begin() + 2, 5, tester_class1);
  } catch (...) {
    catched = true;
  }
  EXPECT_EQ(catched, true);
  catched = false;

  try {
    stdvec2.insert(stdvec2.begin() + 2, 5, tester_class1);
  } catch (...) {
    catched = true;
  }
  EXPECT_EQ(catched, true);
  EXPECT_EQ(compare_to_std(stdvec2, s21vec2, false), true);
  catched = false;

  try {
    s21vec2.insert(s21vec2.begin() + 2, s21vec3.begin(), s21vec3.end());
  } catch (...) {
    catched = true;
  }
  EXPECT_EQ(catched, true);
  catched = false;

  try {
    stdvec2.insert(stdvec2.begin() + 2, stdvec3.begin(), stdvec3.end());
  } catch (...) {
    catched = true;
  }
  EXPECT_EQ(catched, true);
  EXPECT_EQ(compare_to_std(stdvec2, s21vec2, false), true);
  catched = false;

  for (int i = 1; i < 15; ++i) {
    s21vec2[i].n = 1;
    stdvec2[i].n = 1;
  }

  try {
    s21vec2.reserve(100);
  } catch (...) {
    catched = true;
  }
  EXPECT_EQ(catched, true);
  catched = false;

  try {
    s21vec2.reserve(100);
  } catch (...) {
    catched = true;
  }
  EXPECT_EQ(catched, true);
  EXPECT_EQ(compare_to_std(stdvec2, s21vec2, false), true);
  catched = false;
}

// Map tests

TEST(Map, constructor_0) {
  s21::map<int, std::string> s21_map;
  std::map<int, std::string> std_map;
  EXPECT_EQ(s21_map.size(), std_map.size());
  EXPECT_EQ(s21_map.empty(), std_map.empty());
}

TEST(Map, constructor_1) {
  s21::map<int, std::string> s21_map = {
      {1, "first"}, {2, "second"}, {3, "third"}, {4, "fourst"}};

  std::map<int, std::string> std_map = {
      {1, "first"}, {2, "second"}, {3, "third"}, {4, "fourst"}};

  EXPECT_EQ(s21_map.size(), std_map.size());
  EXPECT_EQ(s21_map.empty(), std_map.empty());

  auto stdIt = std_map.begin();
  auto s21It = s21_map.begin();
  while (stdIt != std_map.end()) {
    auto x = *s21It;
    auto y = *stdIt;
    EXPECT_EQ(x.first, y.first);
    EXPECT_EQ(x.second, y.second);
    stdIt++;
    s21It++;
  }
}

TEST(Map, constructor_2) {
  s21::map<int, std::string> s21_map = {
      {1, "first"}, {2, "second"}, {3, "third"}, {4, "fourst"}};

  s21::map<int, std::string> s21_map_copy(s21_map);

  EXPECT_EQ(s21_map.size(), s21_map_copy.size());
  EXPECT_EQ(s21_map.empty(), s21_map_copy.empty());

  auto s21It = s21_map_copy.begin();
  auto s21It_copy = s21_map.begin();
  while (s21It != s21_map_copy.end()) {
    auto x = *s21It;
    auto y = *s21It_copy;
    EXPECT_EQ(x.first, y.first);
    EXPECT_EQ(x.second, y.second);
    s21It_copy++;
    s21It++;
  }
}

TEST(Map, constructor_3) {
  s21::map<int, std::string> s21_map = {
      {1, "first"}, {2, "second"}, {3, "third"}, {4, "fourst"}};

  s21::map<int, std::string> s21_map_moved(std::move(s21_map));

  EXPECT_EQ(s21_map_moved.size(), 4);
  EXPECT_EQ(s21_map_moved.empty(), false);

  EXPECT_EQ(s21_map.size(), 0);
  EXPECT_EQ(s21_map.empty(), true);
}

TEST(Map, insert_0) {
  s21::map<int, std::string> s21_map;
  std::map<int, std::string> std_map;

  auto x = s21_map.insert(1, "first");
  auto y = std_map.insert({1, "first"});
  EXPECT_EQ(x.second, y.second);

  auto z = s21_map.insert(1, "first");
  auto w = std_map.insert({1, "first"});
  EXPECT_EQ(z.second, w.second);
}

TEST(Map, insert_or_assign_0) {
  s21::map<int, std::string> s21_map;
  std::map<int, std::string> std_map;

  auto x = s21_map.insert_or_assign(1, "first");
  auto y = std_map.insert_or_assign(1, "first");
  EXPECT_EQ(x.second, y.second);

  auto z = s21_map.insert_or_assign(1, "first");
  auto w = std_map.insert_or_assign(1, "first");
  EXPECT_EQ(z.second, w.second);
}

TEST(Map, erase_0) {
  s21::map<int, std::string> s21_map = {
      {1, "first"}, {2, "second"}, {3, "third"}, {4, "fourst"}};

  std::map<int, std::string> std_map = {
      {1, "first"}, {2, "second"}, {3, "third"}, {4, "fourst"}};

  auto x = s21_map.find(3);
  auto y = std_map.find(3);

  s21_map.erase(x);
  std_map.erase(y);

  EXPECT_EQ(s21_map.size(), std_map.size());
  EXPECT_EQ(s21_map.empty(), std_map.empty());

  auto stdIt = std_map.begin();
  auto s21It = s21_map.begin();
  while (stdIt != std_map.end()) {
    auto x = *s21It;
    auto y = *stdIt;
    EXPECT_EQ(x.first, y.first);
    EXPECT_EQ(x.second, y.second);
    stdIt++;
    s21It++;
  }
}

TEST(Map, swap) {
  s21::map<int, std::string> s21_map_one = {
      {1, "first"}, {2, "second"}, {3, "third"}, {4, "fourst"}};

  s21::map<int, std::string> s21_map_two = {
      {5, "fifth"}, {6, "sixth"}, {7, "seventh"}, {8, "eighth"}};

  s21_map_one.swap(s21_map_two);

  auto i = 1;
  for (auto it = s21_map_two.begin(); it != s21_map_two.end(); it++) {
    EXPECT_EQ((*it).first, i++);
  }

  for (auto it = s21_map_one.begin(); it != s21_map_one.end(); it++) {
    EXPECT_EQ((*it).first, i++);
  }
}

TEST(Map, merge) {
  s21::map<int, std::string> test{{1, "apple"}, {2, "zorro"},  {4, "batman"},
                                  {5, "pear"},  {8, "alpaca"}, {10, "banana"}};

  s21::map<int, std::string> ma{{1, "apple"}, {5, "pear"}, {10, "banana"}};

  s21::map<int, std::string> mb{
      {2, "zorro"}, {4, "batman"}, {5, "X"}, {8, "alpaca"}};

  s21::map<int, std::string> u;
  u.merge(ma);
  EXPECT_EQ(ma.size(), 0);
  u.merge(mb);
  EXPECT_EQ(mb.size(), 1);
  EXPECT_EQ(mb.at(5), "X");
  EXPECT_EQ(mb[5], "X");

  auto testIt = test.begin();
  auto s21It = u.begin();
  while (s21It != u.end()) {
    auto x = *s21It;
    auto y = *testIt;
    EXPECT_EQ(x.first, y.first);
    EXPECT_EQ(x.second, y.second);
    testIt++;
    s21It++;
  }
}

TEST(Map, emplace) {
  s21::map<int, std::string> s21_map;

  s21_map.emplace(1, "first");

  s21_map.emplace(std::piecewise_construct, std::forward_as_tuple(2),
                  std::forward_as_tuple("second"));

  EXPECT_EQ(s21_map.size(), 2);
}

// Set tests

TEST(Set, constructor_0) {
  s21::set<int> s21_set;
  std::set<int> std_set;
  EXPECT_EQ(s21_set.size(), std_set.size());
  EXPECT_EQ(s21_set.empty(), std_set.empty());
}

TEST(Set, constructor_1) {
  s21::set<int> s21_set = {1, 3, 6, 7, 8};
  std::set<int> std_set = {1, 3, 6, 7, 8};

  EXPECT_EQ(s21_set.size(), std_set.size());
  EXPECT_EQ(s21_set.empty(), std_set.empty());

  auto stdIt = std_set.begin();
  auto s21It = s21_set.begin();
  while (stdIt != std_set.end()) {
    auto x = *s21It;
    auto y = *stdIt;
    EXPECT_EQ(x, y);
    stdIt++;
    s21It++;
  }
}

TEST(Set, constructor_2) {
  s21::set<int> s21_set = {1, 3, 6, 7, 8};

  s21::set<int> s21_set_copy(s21_set);

  EXPECT_EQ(s21_set.size(), s21_set_copy.size());
  EXPECT_EQ(s21_set.empty(), s21_set_copy.empty());

  auto s21It = s21_set_copy.begin();
  auto s21It_copy = s21_set.begin();
  while (s21It != s21_set_copy.end()) {
    auto x = *s21It;
    auto y = *s21It_copy;
    EXPECT_EQ(x, y);
    s21It_copy++;
    s21It++;
  }
}

TEST(Set, constructor_3) {
  s21::set<int> s21_set = {1, 3, 6, 7, 8};

  s21::set<int> s21_set_moved(std::move(s21_set));

  EXPECT_EQ(s21_set_moved.size(), 5);
  EXPECT_EQ(s21_set_moved.empty(), false);

  EXPECT_EQ(s21_set.size(), 0);
  EXPECT_EQ(s21_set.empty(), true);
}

TEST(Set, insert_0) {
  s21::set<int> s21_set;
  std::set<int> std_set;

  auto x = s21_set.insert(1);
  auto y = std_set.insert(1);
  EXPECT_EQ(*x.first, *y.first);

  auto z = s21_set.insert(1);
  auto w = std_set.insert(1);
  EXPECT_EQ(*z.first, *w.first);
}

TEST(Set, erase_0) {
  s21::set<int> s21_set = {1, 3, 6, 7, 8};
  std::set<int> std_set = {1, 3, 6, 7, 8};

  auto x = s21_set.find(3);
  auto y = std_set.find(3);

  s21_set.erase(x);
  std_set.erase(y);

  EXPECT_EQ(s21_set.size(), std_set.size());
  EXPECT_EQ(s21_set.empty(), std_set.empty());

  auto stdIt = std_set.begin();
  auto s21It = s21_set.begin();
  while (stdIt != std_set.end()) {
    auto x = *s21It;
    auto y = *stdIt;
    EXPECT_EQ(x, y);
    stdIt++;
    s21It++;
  }
}

TEST(Set, swap) {
  s21::set<int> s21_set_one = {1, 2, 3};
  s21::set<int> s21_set_two = {4, 5, 6};

  s21_set_one.swap(s21_set_two);

  auto i = 1;
  for (auto it = s21_set_two.begin(); it != s21_set_two.end(); it++) {
    EXPECT_EQ(*it, i++);
  }

  for (auto it = s21_set_one.begin(); it != s21_set_one.end(); it++) {
    EXPECT_EQ(*it, i++);
  }
}

TEST(Set, merge) {
  s21::set<int> test = {1, 3, 6, 7, 8};

  s21::set<int> ma{1, 3, 6};

  s21::set<int> mb{3, 7, 8};

  s21::set<int> u;
  u.merge(ma);
  EXPECT_EQ(ma.size(), 0);
  u.merge(mb);
  EXPECT_EQ(mb.size(), 1);

  auto testIt = test.begin();
  auto s21It = u.begin();
  while (s21It != u.end()) {
    auto x = *s21It;
    auto y = *testIt;
    EXPECT_EQ(x, y);
    testIt++;
    s21It++;
  }
}

TEST(Set, emplace) {
  s21::set<std::string> s21_set;

  s21_set.emplace("first");
  s21_set.emplace("second");

  EXPECT_EQ(s21_set.size(), 2);
}

// Multiset tests

TEST(Multiset, constructor_0) {
  s21::multiset<int> s21_set;
  std::multiset<int> std_set;
  EXPECT_EQ(s21_set.size(), std_set.size());
  EXPECT_EQ(s21_set.empty(), std_set.empty());
}

TEST(Multiset, constructor_1) {
  s21::multiset<int> s21_set = {1, 3, 6, 6, 7, 8, 8};
  std::multiset<int> std_set = {1, 3, 6, 6, 7, 8, 8};

  EXPECT_EQ(s21_set.size(), std_set.size());
  EXPECT_EQ(s21_set.empty(), std_set.empty());

  auto stdIt = std_set.begin();
  auto s21It = s21_set.begin();
  while (stdIt != std_set.end()) {
    auto x = *s21It;
    auto y = *stdIt;
    EXPECT_EQ(x, y);
    stdIt++;
    s21It++;
  }
}

TEST(Multiset, constructor_2) {
  s21::multiset<int> s21_set = {1, 3, 6, 6, 7, 8, 8};

  s21::multiset<int> s21_set_copy(s21_set);

  EXPECT_EQ(s21_set.size(), s21_set_copy.size());
  EXPECT_EQ(s21_set.empty(), s21_set_copy.empty());

  auto s21It = s21_set_copy.begin();
  auto s21It_copy = s21_set.begin();
  while (s21It != s21_set_copy.end()) {
    auto x = *s21It;
    auto y = *s21It_copy;
    EXPECT_EQ(x, y);
    s21It_copy++;
    s21It++;
  }
}

TEST(Multiset, constructor_3) {
  s21::multiset<int> s21_set = {1, 3, 6, 6, 7, 8, 8};

  s21::multiset<int> s21_set_moved(std::move(s21_set));

  EXPECT_EQ(s21_set_moved.size(), 7);
  EXPECT_EQ(s21_set_moved.empty(), false);

  EXPECT_EQ(s21_set.size(), 0);
  EXPECT_EQ(s21_set.empty(), true);
}

TEST(Multiset, insert_0) {
  s21::multiset<int> s21_set;
  std::multiset<int> std_set;

  auto x = s21_set.insert(1);
  auto y = std_set.insert(1);
  EXPECT_EQ(*x, *y);

  auto z = s21_set.insert(1);
  auto w = std_set.insert(1);
  EXPECT_EQ(*z, *w);
}

TEST(Multiset, erase_0) {
  s21::multiset<int> s21_set = {1, 3, 6, 6, 7, 8, 8};
  std::multiset<int> std_set = {1, 3, 6, 6, 7, 8, 8};

  auto x = s21_set.find(3);
  auto y = std_set.find(3);

  s21_set.erase(x);
  std_set.erase(y);

  EXPECT_EQ(s21_set.size(), std_set.size());
  EXPECT_EQ(s21_set.empty(), std_set.empty());

  auto stdIt = std_set.begin();
  auto s21It = s21_set.begin();
  while (stdIt != std_set.end()) {
    auto x = *s21It;
    auto y = *stdIt;
    EXPECT_EQ(x, y);
    stdIt++;
    s21It++;
  }
}

TEST(Multiset, swap) {
  s21::multiset<int> s21_set_one = {1, 2, 3};
  s21::multiset<int> s21_set_two = {4, 5, 6};

  s21_set_one.swap(s21_set_two);

  auto i = 1;
  for (auto it = s21_set_two.begin(); it != s21_set_two.end(); it++) {
    EXPECT_EQ(*it, i++);
  }

  for (auto it = s21_set_one.begin(); it != s21_set_one.end(); it++) {
    EXPECT_EQ(*it, i++);
  }
}

TEST(Multiset, merge) {
  s21::multiset<int> test = {1, 3, 3, 6, 7, 8};

  s21::multiset<int> ma{1, 3, 6};

  s21::multiset<int> mb{3, 7, 8};

  s21::multiset<int> u;
  u.merge(ma);
  EXPECT_EQ(ma.size(), 0);
  u.merge(mb);
  EXPECT_EQ(mb.size(), 0);

  auto testIt = test.begin();
  auto s21It = u.begin();
  while (s21It != u.end()) {
    auto x = *s21It;
    auto y = *testIt;
    EXPECT_EQ(x, y);
    testIt++;
    s21It++;
  }
}

TEST(Multiset, count) {
  s21::multiset<int> test = {1, 3, 3, 6, 7, 8};

  EXPECT_EQ(test.count(0), 0);
  EXPECT_EQ(test.count(1), 1);
  EXPECT_EQ(test.count(3), 2);
}

TEST(Multiset, lower_bound) {
  s21::multiset<int> s21_set = {1, 3, 3, 6, 7, 8};

  EXPECT_EQ(*s21_set.lower_bound(1), 1);
  EXPECT_EQ(*s21_set.lower_bound(4), 6);
  EXPECT_EQ(*s21_set.lower_bound(6), 6);
}

TEST(Multiset, upper_bound) {
  s21::multiset<int> s21_set = {1, 3, 3, 6, 7, 8};

  EXPECT_EQ(*s21_set.upper_bound(-1), 1);
  EXPECT_EQ(*s21_set.upper_bound(4), 6);
  EXPECT_EQ(*s21_set.upper_bound(7), 8);
}

TEST(Multiset, emplace) {
  s21::set<std::string> s21_set;

  s21_set.emplace("first");
  s21_set.emplace("second");
  s21_set.emplace("second");

  EXPECT_EQ(s21_set.size(), 2);
}

TEST(list, constructor_0) {
  s21::list<int> a;
  for (int i = 1; i < 5; i++) {
    a.push_back(i);
  }

  s21::list<int> a_copy(a);

  s21::list<int>::iterator j = a.begin();
  for (s21::list<int>::iterator i = a_copy.begin(); i != a_copy.end();
       i++, j++) {
    EXPECT_EQ(*i, *j);
  }
}

TEST(list, constructor_1) {
  s21::list<int> b({1, 2, 3, 4});
  std::list<int> a({1, 2, 3, 4});

  std::list<int>::iterator j = a.begin();
  for (s21::list<int>::iterator i = b.begin(); i != b.end(); i++, j++) {
    EXPECT_EQ(*i, *j);
  }
}

TEST(list, constructor_2) {
  std::list<int> a;
  s21::list<int> b;

  for (int i = 1; i < 5; i++) {
    a.push_back(i);
    b.push_back(i);
  }

  std::list<int> curr(std::move(a));
  s21::list<int> s21_curr(std::move(b));

  std::list<int>::iterator j = curr.begin();
  for (s21::list<int>::iterator i = s21_curr.begin(); i != s21_curr.end();
       i++, j++) {
    EXPECT_EQ(*i, *j);
  }
}

TEST(list, eq) {
  s21::list<int> list_1 = {1, 2, 3, 4, 5};
  s21::list<int> list_2 = {1, 2, 3, 4, 5};

  EXPECT_EQ(list_1 == list_2, true);

  list_1.push_front(1);

  EXPECT_EQ(list_1 == list_2, false);
}

TEST(list, push_0) {
  std::list<int> a;
  s21::list<int> b;
  for (int i = 100; i < 105; i++) {
    b.push_front(i);
    a.push_front(i);
  }
  std::list<int>::iterator j = a.end();
  s21::list<int>::iterator k = b.end();
  for (int n = 0; n < 10; n++) {
    EXPECT_EQ(*k, *j);
    k++;
    j++;
  }
}

TEST(list, push_1) {
  std::list<int> a;
  s21::list<int> b;

  for (int i = 2; i < 7; i++) {
    a.push_back(i);
    b.push_back(i);
  }

  std::list<int>::iterator j = a.begin();
  for (s21::list<int>::iterator i = b.begin(); i != b.end(); i++, j++) {
    EXPECT_EQ(*i, *j);
  }
}

TEST(list, push_2) {
  std::list<int> a;
  s21::list<int> b;

  for (int i = 2; i < 7; i++) {
    a.push_back(i);
    b.push_back(i);
  }

  std::list<int>::iterator j = a.begin();
  for (s21::list<int>::iterator i = b.begin(); i != b.end(); i++, j++) {
    EXPECT_EQ(*i, *j);
  }
}

TEST(list, pop_0) {
  std::list<int> a;
  s21::list<int> b;

  for (int i = 0; i < 4; i++) {
    a.push_back(i);
    b.push_back(i);
  }
  a.pop_back();
  b.pop_back();

  std::list<int>::iterator j = a.begin();
  for (s21::list<int>::iterator i = b.begin(); i != b.end(); i++, j++) {
    EXPECT_EQ(*i, *j);
  }
}

TEST(list, pop_1) {
  std::list<int> a;
  s21::list<int> b;

  for (int i = 0; i < 4; i++) {
    a.push_back(i);
    b.push_back(i);
  }

  a.pop_back();
  b.pop_back();

  std::list<int>::iterator j = a.begin();
  for (s21::list<int>::iterator i = b.begin(); i != b.end(); i++, j++) {
    EXPECT_EQ(*i, *j);
  }
}

TEST(list, pop_2) {
  std::list<int> a;
  s21::list<int> b;

  for (int i = 3; i < 4; i++) {
    a.push_front(i);
    b.push_front(i);
  }

  a.pop_back();
  b.pop_back();

  std::list<int>::iterator j = a.begin();
  s21::list<int>::iterator i = b.begin();

  EXPECT_EQ(*i, *j);
}

TEST(list, pop_3) {
  std::list<int> a;
  s21::list<int> b;

  for (int i = 3; i < 4; i++) {
    a.push_front(i);
    b.push_front(i);
  }

  a.pop_front();
  b.pop_front();

  std::list<int>::iterator j = a.begin();
  s21::list<int>::iterator i = b.begin();

  EXPECT_EQ(*i, *j);
}

TEST(list, insert_0) {
  std::list<int> a;
  s21::list<int> b;

  for (int i = 1; i < 4; i++) {
    a.push_front(i);
    b.push_front(i);
  }

  std::list<int>::iterator it = a.begin();
  s21::list<int>::iterator s21_it = b.begin();
  it++;
  a.insert(it, 111);
  s21_it++;
  b.insert(s21_it, 111);

  s21::list<int>::iterator j = b.begin();
  for (std::list<int>::iterator i = a.begin(); i != a.end(); i++, j++) {
    EXPECT_EQ(*i, *j);
  }
}

TEST(list, erase_0) {
  std::list<int> a;
  s21::list<int> b;

  for (int i = 1; i < 4; i++) {
    a.push_front(i);
    b.push_front(i);
  }

  std::list<int>::iterator j = a.begin();
  s21::list<int>::iterator i = b.begin();

  j++;
  a.erase(j);
  i++;
  b.erase(i);

  j = a.begin();
  i = b.begin();
  for (int n = 0; n < 10; n++, i++, j++) {
    EXPECT_EQ(*i, *j);
  }
}

TEST(list, erase_1) {
  std::list<int> a;
  s21::list<int> b;

  for (int i = 100; i < 110; i++) {
    a.push_front(i);
    b.push_front(i);
  }

  s21::list<int>::iterator s21_it = b.begin();
  std::list<int>::iterator it = a.begin();

  a.erase(it);
  b.erase(s21_it);

  std::list<int>::iterator j = a.begin();
  s21::list<int>::iterator i = b.begin();
  for (int n = 0; n < 10; n++, i++, j++) {
    EXPECT_EQ(*i, *j);
  }
}

TEST(list, erase_2) {
  std::list<int> a;
  s21::list<int> b;

  for (int i = 100; i < 110; i++) {
    a.push_front(i);
    b.push_front(i);
  }

  s21::list<int>::iterator i = b.end();
  std::list<int>::iterator j = a.end();
  j--;
  a.erase(j);
  i--;
  b.erase(i);

  i = b.begin();
  j = a.begin();

  for (int n = 0; n < 10; n++, i++, j++) {
    EXPECT_EQ(*i, *j);
  }
}

TEST(list, erase_3) {
  s21::list<int> b = {1, 2, 3, 4, 5, 6};
  std::list<int> a = {1, 2, 3, 4, 5, 6};

  s21::list<int>::iterator s21_it = b.begin();
  std::list<int>::iterator it = a.begin();

  it++;
  s21_it++;

  a.erase(it);
  b.erase(s21_it);

  std::list<int>::iterator j = a.begin();
  for (s21::list<int>::iterator i = b.begin(); i != b.end(); i++, j++) {
    EXPECT_EQ(*i, *j);
  }
}

TEST(list, erase_4) {
  std::list<int> a;
  s21::list<int> b;

  for (int i = 3; i < 4; i++) {
    a.push_front(i);
    b.push_front(i);
  }

  s21::list<int>::iterator s21_it = b.begin();
  std::list<int>::iterator it = a.begin();

  a.erase(it);
  b.erase(s21_it);

  std::list<int>::iterator j = a.begin();
  s21::list<int>::iterator i = b.begin();

  EXPECT_EQ(*i, *j);
}

TEST(list, sort_0) {
  std::list<int> a = {1, 2, 3, 4, -1, -2, -33, 44};
  s21::list<int> b = {1, 2, 3, 4, -1, -2, -33, 44};

  a.sort();
  b.sort();

  std::list<int>::iterator j = a.begin();
  s21::list<int>::iterator i = b.begin();
  for (int n = 0; n < 20; n++) {
    EXPECT_EQ(*i, *j);
    i++;
    j++;
  }
}

TEST(list, unique) {
  std::list<int> a = {1, 2, 3, 3, 3, 3, 4, -1, -2, -33, 44, 3, 3, 3};
  s21::list<int> b = {1, 2, 3, 3, 3, 3, 4, -1, -2, -33, 44, 3, 3, 3};

  a.unique();
  b.unique();

  std::list<int>::iterator j = a.begin();
  for (s21::list<int>::iterator i = b.begin(); i != b.end(); i++, j++) {
    EXPECT_EQ(*i, *j);
  }
}

TEST(list, reverse) {
  std::list<int> a = {1, 2, 3, 3, 3, 3, 4, -1, -2, -33, 44, 3, 3, 3};
  s21::list<int> b = {1, 2, 3, 3, 3, 3, 4, -1, -2, -33, 44, 3, 3, 3};

  a.reverse();
  b.reverse();

  std::list<int>::iterator j = a.begin();
  for (s21::list<int>::iterator i = b.begin(); i != b.end(); i++, j++) {
    EXPECT_EQ(*i, *j);
  }
}

TEST(list, merge) {
  std::list<int> a = {1, 2, 3}, aa = {4, 5, 6};
  s21::list<int> b = {1, 2, 3}, bb = {4, 5, 6};

  b.merge(bb);
  a.merge(aa);

  std::list<int>::iterator j = a.begin();
  s21::list<int>::iterator i = b.begin();
  for (int n = 0; n < 20; n++, i++, j++) {
    EXPECT_EQ(*i, *j);
  }
}

TEST(list, swap) {
  s21::list<int> a = {5, 6, 3};
  s21::list<int> b = {1, 2, 3, 4, 5, 6};
  a.swap(b);

  std::list<int> aa = {5, 6, 3};
  std::list<int> bb = {1, 2, 3, 4, 5, 6};
  aa.swap(bb);

  std::list<int>::iterator j = aa.begin();
  s21::list<int>::iterator i = a.begin();
  for (int n = 0; n < 10; n++, i++, j++) {
    EXPECT_EQ(*i, *j);
  }
}

TEST(list, splice) {
  std::list<int> a = {1, 2, 3}, aa = {4, 5, 6};
  s21::list<int> b = {1, 2, 3}, bb = {4, 5, 6};

  s21::list<int>::iterator b_it = b.begin();
  std::list<int>::iterator a_it = a.begin();

  a_it++;
  b_it++;

  b.splice(b_it, bb);
  a.splice(a_it, aa);

  std::list<int>::iterator j = a.begin();
  s21::list<int>::iterator i = b.begin();
  for (int n = 0; n < 20; n++, i++, j++) {
    EXPECT_EQ(*i, *j);
  }
}

TEST(list, front_back) {
  std::list<int> a = {1, 2, 3};
  s21::list<int> b = {1, 2, 3};

  EXPECT_EQ(b.front(), a.front());
  EXPECT_EQ(b.back(), a.back());
}

TEST(list, emplace) {
  s21::list<int> a;
  a.emplace_front(3, 2, 1);
  a.emplace_back(9, 10, 11, 12);
  s21::list<int>::iterator it = a.begin();
  it++;
  it++;
  it++;
  a.emplace(it, 4, 5, 6, 7, 8);

  it = a.begin();
  for (int n = 1; it != a.end(); n++, it++) EXPECT_EQ(*it, n);
  EXPECT_EQ(a.max_size(), a.max_size());
}

TEST(queu, constructor_0) {
  std::size_t num = 10;
  s21::queue<int> a(num);
  ASSERT_TRUE(a.size() == 10);
}

TEST(queu, constructor_1) {
  s21::queue<int> a = {1, 2, 3, 4};
  ASSERT_TRUE(a.size() == 4);
  ASSERT_TRUE(a.front() == 1);
  ASSERT_TRUE(a.back() == 4);
}

TEST(queu, constructor_2) {
  s21::queue<int> a = {};
  ASSERT_TRUE(a.size() == 0);
}

TEST(queu, constructor_3) {
  s21::queue<int> a = {1, 2, 3, 4};
  s21::queue<int> a2(a);
  ASSERT_TRUE(a2.size() == 4);
  ASSERT_TRUE(a2.front() == 1);
  ASSERT_TRUE(a2.back() == 4);
}

TEST(queu, constructor_4) {
  s21::queue<int> a = {1, 2, 3, 4};
  s21::queue<int> a2 = std::move(a);
  ASSERT_TRUE(a2.size() == 4);
  ASSERT_TRUE(a2.front() == 1);
  ASSERT_TRUE(a2.back() == 4);
  ASSERT_TRUE(a.empty());
}

TEST(queue, empty) {
  s21::queue<int> a;
  EXPECT_TRUE(a.empty());
}

TEST(queue, size) {
  s21::queue<int> a = {1, 2, 3};
  EXPECT_EQ(a.size(), 3);
}

TEST(queu, push_pop) {
  s21::queue<int> a = {1, 2, 3, 4, 5, 6};

  ASSERT_TRUE(a.front() == 1);
  a.pop();
  ASSERT_EQ(a.front(), 2);
  a.push(42);

  ASSERT_TRUE(a.front() == 2 && a.back() == 42);
  ASSERT_TRUE(a.size() == 6);
}

TEST(queu, swap) {
  s21::queue<int> a = {1, 2, 3};
  s21::queue<int> a2 = {4, 5, 6, 7};
  a.swap(a2);

  ASSERT_TRUE(a.size() == 4 && a2.size() == 3);
  ASSERT_TRUE(a.front() == 4 && a2.front() == 1);
}

TEST(queue, emplace) {
  s21::queue<int> b = {1, 2, 3};
  b.emplace_back(4);
  ASSERT_EQ(b.back(), 4);
}

TEST(stack, constructor_0) {
  s21::stack<int> b{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  for (int i = 10; i > 0; i--) {
    ASSERT_EQ(b.top(), i);
    b.pop();
  }
}

TEST(stack, constructor_1) { s21::stack<int> a; }

TEST(stack, constructor_2) {
  s21::stack<int> b = {1, 2, 3, 4};
  s21::stack<int> a(b);
  for (int i = 4; i > 0; i--) {
    ASSERT_EQ(a.top(), i);
    a.pop();
    b.pop();
  }
}

TEST(stack, constructor_3) {
  s21::stack<int> a;
  for (int i = 0; i < 100; i++) {
    a.push(i);
  }
  s21::stack<int> copy(a);
  s21::stack<int> move(std::move(a));
  for (int i = 0; i < 100; i++) {
    EXPECT_EQ(copy.top(), move.top());
    copy.pop();
    move.pop();
  }
}

TEST(stack, constructor_4) {
  s21::stack<int> b = {1, 2, 3, 4, 5};
  s21::stack<int> a = b;
  for (int i = 5; i > 0; i--) {
    EXPECT_EQ(a.top(), i);
    a.pop();
  }
}

TEST(stack, empty) {
  s21::stack<int> a;
  EXPECT_TRUE(a.empty());
}

TEST(stack, size) {
  s21::stack<int> a = {1, 2, 3};
  EXPECT_EQ(a.size(), 3);
}

TEST(stack, push_pop_0) {
  s21::stack<int> A;
  std::stack<int> B;
  for (int i = 0; i < 100; i++) {
    A.push(i);
    B.push(i);
  }
  for (int i = 0; i < 100; i++) {
    EXPECT_EQ(A.top(), B.top());
    A.pop();
    B.pop();
  }
}

TEST(stack, push_pop_1) {
  s21::stack<int> A;
  std::stack<int> B;
  for (int i = 0; i < 100; i++) {
    A.push(i);
    B.push(i);
  }
  for (int i = 0; i < 100; i++) {
    EXPECT_EQ(A.top(), B.top());
    A.pop();
    B.pop();
  }
}

TEST(stack, push_pop_2) {
  s21::stack<int> a;
  std::stack<int> b;
  for (int i = 0; i < 1000; i++) {
    a.push(i);
    b.push(i);
  }

  for (int i = 1000 - 1; i >= 0; i--) {
    ASSERT_EQ(a.top(), b.top());
    a.pop();
    b.pop();
  }
}

TEST(stack, swap) {
  s21::stack<int> a = {1, 2, 3};
  s21::stack<int> a2 = {4, 5, 6, 7};
  a.swap(a2);

  ASSERT_TRUE(a.size() == 4 && a2.size() == 3);
  ASSERT_TRUE(a.front() == 4 && a2.front() == 1);
}

TEST(stack, emplace) {
  s21::stack<int> a;
  std::stack<int> b;
  b.push(1);
  b.push(2);
  b.push(3);
  b.push(4);
  b.emplace(5);

  a.push(1);
  a.push(2);
  a.push(3);
  a.push(4);
  a.emplace_front(5);
  for (int i = 5; i > 0; i--) {
    ASSERT_EQ(a.top(), b.top());
    a.pop();
    b.pop();
  }
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

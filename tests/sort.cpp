#include "../compression.hpp"

#include "math.h"

#include <gtest/gtest.h>

using namespace compression;

template <typename local_array_t, u64 local_array_size>
void expect_sorted(const local_array<local_array_t, local_array_size>& a) {
  for (u64 i = 1; i < local_array_size; i++) {
    EXPECT_LE(a.data[i - 1], a.data[i]);
  }
}

template <typename local_array_t, u64 local_array_size>
void generate_test_array(local_array<local_array_t, local_array_size>& a) {
  for (u64 i = 0; i < local_array_size; i++) {
    a.data[i] = i % 2 ? i - i / 2 : i * 3;
  }
}

template <typename local_array_t, u64 local_array_size>
void generate_binary_array(local_array<local_array_t, local_array_size>& a) {
  u64 dim = sqrt(local_array_size);
  for (u64 j = 0; j < dim; j++) {
    for (u64 i = 0; i < dim; i++) {
      u64 index = j * dim + i;
      u64 b = j % 2 == 0;
      u64 c = i % 2 == 1;
      a.data[index] = c & b;
    }
  }
}

TEST(sort_test_suite, one_element) {
  local_array<u64, 1> array;
  array.data[0] = 1;

  array.iterative_inplace_quicksort();

  EXPECT_EQ(array.data[0], 1);
}

TEST(sort_test_suite, ten_element_sanity_check) {
  local_array<u64, 10> array;
  array.data[0] = 10;
  array.data[1] = 9;
  array.data[2] = 8;
  array.data[3] = 1;
  array.data[4] = 3;
  array.data[5] = 2;
  array.data[6] = 1;
  array.data[7] = 5;
  array.data[8] = 6;
  array.data[9] = 7;

  array.iterative_inplace_quicksort();

  expect_sorted(array);
}

TEST(sort_test_suite, already_sorted_array) {
  local_array<u64, 10> array;
  array.data[0] = 1;
  array.data[1] = 2;
  array.data[2] = 3;
  array.data[3] = 4;
  array.data[4] = 5;
  array.data[5] = 6;
  array.data[6] = 7;
  array.data[7] = 8;
  array.data[8] = 9;
  array.data[9] = 10;

  array.iterative_inplace_quicksort();

  expect_sorted(array);
}

TEST(sort_test_suite, reverse_sorted_array) {
  local_array<u64, 10> array;
  array.data[0] = 10;
  array.data[1] = 9;
  array.data[2] = 8;
  array.data[3] = 7;
  array.data[4] = 6;
  array.data[5] = 5;
  array.data[6] = 4;
  array.data[7] = 3;
  array.data[8] = 2;
  array.data[9] = 1;

  array.iterative_inplace_quicksort();

  expect_sorted(array);
}

TEST(sort_test_suite, num_elements_256) {
  local_array<u64, 256> a;
  generate_test_array(a);
  a.iterative_inplace_quicksort();
  expect_sorted(a);
}

TEST(sort_test_suite, num_elements_256_binary) {
  local_array<u64, 256> a;
  generate_binary_array(a);
  a.iterative_inplace_quicksort();
  expect_sorted(a);
}

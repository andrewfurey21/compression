#ifndef __COMPRESSION_HPP_HEADER_GUARD
#define __COMPRESSION_HPP_HEADER_GUARD

#include <compare>
#include <ostream>
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace compression {

using u8 = unsigned char;
using u64 = unsigned long long;

struct file {
  u8 *data = NULL;
  u64 size;

  explicit file() { data = NULL; size = 0; }
  explicit file(const char *name) {
    std::ifstream file_stream(name);
    size = std::filesystem::file_size(name);
    data = (u8 *)malloc(size);
    file_stream.read((char *)data, size);
  }

  ~file() { free(data); }
};

struct letter_count {
  u64 count = 0;
  u8 letter = 0;

  std::strong_ordering operator<=>(const letter_count& other) const {
    return count <=> other.count;
  }

  void inc() { count++; }
  friend std::ostream& operator<<(std::ostream& out, const letter_count& a) {
    out << a.letter << ": " << a.count << "\n";
    return out;
  }
};

template <typename T, u64 num>
struct local_array {
  const u64 size = num;
  T data[num] = {0}; // size must be > 0

  template <typename local_array_t, u64 local_array_size>
  friend std::ostream& operator<<(std::ostream& out, const local_array<local_array_t, local_array_size>& a);
  void swap(u64 i, u64 j);
  void iterative_inplace_quicksort();
};

template <u64 max_size>
struct range_stack {
  local_array<u64, max_size * 2> array;
  u64 pos = 0;
  void push(u64 start, u64 end) {
    assert(pos <= array.size);
    array.data[pos] = start;
    array.data[pos + 1] = end;
    pos += 2;
  }

  void pop(u64 &start, u64 &end) {
    if (pos == 0) {
      start = 0;
      end = 0;
      return;
    }

    u64 _end = array.data[pos - 1];
    u64 _start = array.data[pos - 2];

    start = _start;
    end = _end;

    pos -= 2;
  }

  bool empty() { return pos == 0; }
};


template <typename T, u64 num>
std::ostream& operator<<(std::ostream& out, const local_array<T, num>& a) {
  for (u64 i = 0; i < a.size; i++) {
    out << a.data[i];
    if (i < a.size - 1) out << ", ";
  }
  return out;
}


template <typename T, u64 num>
void local_array<T, num>::swap(u64 i, u64 j) {
  T temp = data[i];
  data[i] = data[j];
  data[j] = temp;
}

template <typename T, u64 num>
void local_array<T, num>::iterative_inplace_quicksort() {
  if (size <= 1) return;
  range_stack<num> stack;
  stack.push(0, size - 1);

  while (!stack.empty()) {
    u64 start, end;
    stack.pop(start, end);

    const u64 initial_start = start;
    const u64 initial_end = end;

    if (start >= end) continue;

    u64 pivot_index = (end - start) / 2 + start;

    while (start < end) {
      while (this->data[start] < this->data[pivot_index] && start <= pivot_index)
        start++;

      while (this->data[pivot_index] <= this->data[end] && end > pivot_index)
        end--;

      if (start == end) { break; }

      swap(start, end);

      if (start == pivot_index) {
        pivot_index = end;
      } else if (end == pivot_index) {
        pivot_index = start;
      }
    }

    if (pivot_index > 0 && initial_start < pivot_index - 1)
      stack.push(initial_start, pivot_index - 1);

    if (initial_end > pivot_index + 1)
      stack.push(pivot_index + 1, initial_end);
  }
}


inline void huffman_byte_encode(const file& decoded, file& encoded) {
  local_array<letter_count, 256> byte_counts;
  for (u64 i = 0; i < 256; i++) {
    byte_counts.data[i].letter = (u8)i;
  }
  for (u64 i = 0; i < decoded.size; i++) {
    byte_counts.data[decoded.data[i]].inc();
  }

  byte_counts.iterative_inplace_quicksort();



}

inline void huffman_byte_decode(const file& encoded, file& decoded) {

}

};

#endif

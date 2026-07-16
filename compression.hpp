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

inline void panic_if(bool expr, const std::string& msg) {
  if (expr) {
    std::cerr << msg << "\n";
    exit(1);
  }
}

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
    if (a.count > 0)
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

template <typename NodeType, u64 num_nodes>
struct binary_heap {
  static_assert(num_nodes > 0);

  local_array<NodeType, num_nodes + 1> array; // zeroth element is empty.
  u64 num_inserted = 0;

  void insert(const NodeType& n) {
    panic_if(num_nodes <= num_inserted, "Too many nodes inserted into heap.");
    num_inserted++;

    u64 node_index = num_inserted;
    array.data[node_index] = n;

    while (node_index > 0) {
      u64 parent_index = node_index / 2;
      if (array.data[parent_index] > array.data[node_index]) {
        array.swap(parent_index, node_index);
        node_index = parent_index;
      } else {
        break;
      }
    }
  }

  NodeType remove() {
    panic_if(num_inserted == 0, "Cannot remove from empty heap.");
    num_inserted--;

    NodeType copy = array.data[1];

    if (num_inserted == 0) return copy;

    array.data[1] = array.data[num_inserted + 1];

    u64 current = 1;
    while (true) {
      u64 parent_1 = current * 2;
      u64 parent_2 = current * 2 + 1;

      if (parent_1 > num_inserted) break;

      if (parent_2 > num_inserted) {
        if (array.data[current] > array.data[parent_1]) {
          array.swap(current, parent_1);
          current = parent_1;
        } else break;
      } else {
        const NodeType& parent_1_data = array.data[parent_1];
        const NodeType& parent_2_data = array.data[parent_2];

        if (parent_1_data <= parent_2_data) {
          if (array.data[current] > parent_1_data) {
            array.swap(current, parent_1);
            current = parent_1;
          } else break;

        } else {
          if (array.data[current] > parent_2_data) {
            array.swap(current, parent_2);
            current = parent_2;
          } else break;

        }
      }
    }

    return copy;
  }

};


inline void huffman_byte_encode(const file& decoded, file& encoded) {
  local_array<letter_count, 256> byte_counts;
  for (u64 i = 0; i < 256; i++) {
    byte_counts.data[i].letter = (u8)i;
  }
  for (u64 i = 0; i < decoded.size; i++) {
    byte_counts.data[decoded.data[i]].inc();
  }

  byte_counts.iterative_inplace_quicksort();
  // std::cout << byte_counts << "\n";

  u64 num_greater_than_zero = 0;
  for (u64 i = 0; i < byte_counts.size; i++) {
    if (byte_counts.data[i].count > 0) num_greater_than_zero++;
  }





}


// first 8 * 256 bytes will be ascii table, rest is the compressed file.
inline void huffman_byte_decode(const file& encoded, file& decoded) {

}

};

#endif

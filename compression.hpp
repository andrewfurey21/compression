#ifndef __COMPRESSION_HPP_HEADER_GUARD
#define __COMPRESSION_HPP_HEADER_GUARD

#include <compare>
#include <ostream>
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <bitset>
#include <cstring>

namespace compression {

using u8 = unsigned char;
using u64 = unsigned long long;
using f64 = double;

inline void panic_if(bool expr, const std::string& msg) {
  if (expr) {
    std::cerr << msg << "\n";
    exit(1);
  }
}

struct file {
  u8 *data = nullptr;
  const char * name;
  u64 size;

  explicit file(const char *name) : data(nullptr), size(0), name(name) {}

  static bool compare_equal(const file& a, const file& b) {
    panic_if(a.size != b.size, "File sizes are not the same.");
    for (u64 i = 0; i < a.size; i++) {
      if (a.data[i] != b.data[i]) return false;
    }
    return true;
  }

  void read() {
    std::ifstream file_stream(name);
    size = std::filesystem::file_size(name);
    data = (u8 *)malloc(size);
    file_stream.read((char *)data, size);
  }

  void alloc(u64 num_bytes) {
    panic_if(data != nullptr, "Overwriting file buffer.");
    data = (u8 *)malloc(num_bytes);
    size = num_bytes;
  }

  void write() {
    panic_if(strlen(name) == 0, "Cannot write to file with emptry string as file name.");
    std::ofstream file_stream(name);
    file_stream.write((const char*)data, size);
  }

  friend std::ostream& operator<<(std::ostream& os, const file& f) {
    os.write((const char*)f.data, f.size);
    return os;
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

    if (a.count > 0) {
      if (a.letter == 0) {
        out << "empty" << ": " << a.count << "\n";
      } else if (a.letter == '\n') {
        out << "newline" << ": " << a.count << "\n";
      } else {
        out << a.letter << ": " << a.count << "\n";
      }
    }
    return out;
  }
};

template <typename T, u64 num>
struct local_array {
  const u64 size = num;
  T data[num] = {0}; // size must be > 0
  u64 length = 0;

  template <typename local_array_t, u64 local_array_size>
  friend std::ostream& operator<<(std::ostream& out, const local_array<local_array_t, local_array_size>& a);
  void swap(u64 i, u64 j);
  void iterative_inplace_quicksort();
  void push_back(const T& x) {
    panic_if(length >= num, "array out of bounds");
    data[length++] = x;
  }
};

template <u64 max_size>
struct range_stack {
  local_array<u64, max_size * 2> array;
  u64 length = 0;
  void push(u64 start, u64 end) {
    assert(length <= array.size);
    array.data[length] = start;
    array.data[length + 1] = end;
    length += 2;
  }

  void pop(u64 &start, u64 &end) {
    if (length == 0) {
      start = 0;
      end = 0;
      return;
    }

    u64 _end = array.data[length - 1];
    u64 _start = array.data[length - 2];

    start = _start;
    end = _end;

    length -= 2;
  }

  bool empty() { return length == 0; }
};


template <typename T, u64 num>
std::ostream& operator<<(std::ostream& out, const local_array<T, num>& a) {
  for (u64 i = 0; i < a.length; i++) {
    out << i << ": " << a.data[i] << "  ";
    // if (i < a.size - 1) out << ", ";
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
  u64 max_nodes = num_nodes;

  void insert(const NodeType& n) {
    panic_if(num_nodes <= num_inserted, "Too many nodes inserted into heap.");
    num_inserted++;

    u64 node_index = num_inserted;
    array.data[node_index] = n;

    while (node_index > 1) {
      u64 parent_index = node_index / 2;
      if (array.data[parent_index] > array.data[node_index]) {
        array.swap(parent_index, node_index);
        node_index = parent_index;
      } else {
        break;
      }
    }
  }

  const NodeType& top() {
    return array.data[1];
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


template <typename T, u64 max_size>
struct fixed_queue {
  static_assert(max_size > 0);
  local_array<T, max_size> array;
  u64 start;
  u64 end;
  fixed_queue() : start{0}, end{0} {}
  void insert(const T& x) {
    panic_if(num_elements() == max_size, "Cannot insert in full queue.");
    array.data[end] = x;
    wrap_inc(end);
  }

  void wrap_inc(u64 &y) {
    u64 temp = y + 1;
    if (temp >= max_size) temp = 0;
    y = temp;
  }

  T pop() {
    panic_if(num_elements() == 0, "Cannot pop from queue with zero elements.");
    T x = array.data[start];
    wrap_inc(start);
    return x;
  }

  u64 num_elements() {
    if (end < start) {
      return (max_size - start) + end + 1;
    } else {
      return (end - start);
    }
  }
};

struct code {
  u64 bits;
  char letter;
  u8 length;
  u64 count;
};

inline std::ostream& operator<<(std::ostream& os, const code& c) {
  std::cout << c.letter << ": " << "length " << (u64)c.length << ", " << std::bitset<64>(c.bits) << "\n";
  return os;
}

template <u64 max_nodes>
struct binary_tree {
  struct node {
    node *left;
    node *right;
    letter_count count;

    std::strong_ordering operator<=>(const node& other) const {
      return count <=> other.count;
    }
  };

  struct node_ptr {
    node *p;
    std::strong_ordering operator<=>(const node_ptr& other) const {
      return p->count <=> other.p->count;
    }
  };

  struct {
    node memory[max_nodes + 1] = {0};
    u64 num_nodes = 1;
    node_ptr new_node(const node_ptr& left, const node_ptr& right, letter_count count) {
      assert(num_nodes < max_nodes);
      node *n = memory + num_nodes;
      node_ptr p{n};
      n->left = left.p;
      n->right = right.p;
      n->count = count;
      num_nodes++;
      return p;
    }
  } arena;

  node_ptr new_node(const node_ptr& left, const node_ptr& right, letter_count count) {
    return arena.new_node(left, right, count);
  }

  static void print_tree(const node_ptr& n) {
    std::cout << "Count: " << n.p->count << "\n";
    if (n.p->left == nullptr) std::cout << "empty left node\n";
    else {
      std::cout << "Left:\n";
      print_tree(node_ptr{n.p->left});
    }

    if (n.p->right == nullptr) std::cout << "empty right node\n";
    else {
      std::cout << "Right:\n";
      print_tree(node_ptr{n.p->right});
    }
  }

  template <u64 num_letters>
  local_array<code, num_letters> get_codes(const local_array<letter_count, num_letters>& original_counts, const node_ptr& top_of_tree) {

    // this could probably be done better with less queues.
    local_array<code, num_letters> codes;
    fixed_queue<node_ptr, num_letters> node_queue;
    fixed_queue<u64, num_letters> bit_queue;
    fixed_queue<u64, num_letters> depth_queue;

    node_queue.insert(node_ptr{top_of_tree.p->left});
    node_queue.insert(node_ptr{top_of_tree.p->right});
    bit_queue.insert(0);
    bit_queue.insert(1);
    depth_queue.insert(1);
    depth_queue.insert(1);

    while (node_queue.num_elements() > 0) {
      node_ptr node = node_queue.pop();
      u64 bits = bit_queue.pop();
      u64 depth = depth_queue.pop();
      if (node.p->left == nullptr && node.p->right == nullptr) {
        // valid letter
        codes.push_back(code{bits, (char)node.p->count.letter, (u8)depth, node.p->count.count});
        continue;
      }

      if (node.p->left != nullptr) {
        u64 new_bits = (1 << depth) | bits;
        bit_queue.insert(new_bits);
        depth_queue.insert(depth + 1);
        node_queue.insert(node_ptr{node.p->left});
      }

      if (node.p->right != nullptr) {
        bit_queue.insert(bits);
        depth_queue.insert(depth + 1);
        node_queue.insert(node_ptr{node.p->right});
      }
    }

    return codes;
  }

};

template <u64 num>
u64 compute_length_of_compressed_file(const local_array<code, num>& codes) {
  u64 total_num_bits = 0;
  for (u64 i = 0; i < codes.length; i++) {
    total_num_bits += codes.data[i].count * (u64)codes.data[i].length;

  }
  return (total_num_bits + 8 - 1) / 8;
}

template <u64 num>
u64 compute_length_of_decompressed_file(const local_array<code, num>& codes) {
  u64 total_num_bytes = 0;
  for (u64 i = 0; i < codes.length; i++) {
    total_num_bytes += codes.data[i].count;
  }
  return total_num_bytes;
}

inline void get_counts(local_array<letter_count, 256>& counts, const file& f) {
  for (u64 i = 0; i < 256; i++) {
    counts.data[i].letter = (u8)i;
  }

  for (u64 i = 0; i < f.size; i++) {
    counts.data[f.data[i]].inc();
  }
}

inline void huffman_encode_using_binary_heap(const file& decoded, file& encoded) {
  constexpr u64 max_table_size = 256;
  local_array<letter_count, max_table_size> byte_counts;
  get_counts(byte_counts, decoded);


  using tree_t = binary_tree<max_table_size * 2>;
  tree_t tree; // 256 * 2 - 1 is max needed, if all bytes are equally likely, +1 for a garbage node. so 256 * 2 in total.
  binary_heap<tree_t::node_ptr, max_table_size> heap;
  for (u64 i = 0; i < byte_counts.size; i++) {
    if (byte_counts.data[i].count > 0) {
      tree_t::node_ptr n = tree.new_node(tree_t::node_ptr{nullptr}, tree_t::node_ptr{nullptr}, byte_counts.data[i]);
      heap.insert(n);
    }
  }

  while (heap.top().p->count.count != decoded.size) { // lol looks awful.
    tree_t::node_ptr p1 = heap.remove();
    tree_t::node_ptr p2 = heap.remove();
    letter_count total_count = {p1.p->count.count + p2.p->count.count, 0};
    tree_t::node_ptr n = tree.new_node(p1, p2, total_count);
    heap.insert(n);
  }

  tree_t::node_ptr top_of_tree = heap.remove();
  assert(heap.num_inserted == 0);

  local_array codes = tree.get_codes(byte_counts, top_of_tree);
  u64 num_bytes = compute_length_of_compressed_file(codes) + max_table_size * 8;

  std::cout << "compression ratio: " << (f64)(num_bytes) / decoded.size << "\n";

  encoded.alloc(num_bytes);
  //header
  u64 *header = reinterpret_cast<u64 *>(encoded.data);
  for (u64 i = 0; i < max_table_size; i++) {
    header[i] = byte_counts.data[i].count;
  }

  u64 encoded_index = max_table_size * 8;
  u64 bit_counter = 8;
  u8 current_byte = 0;
  for (u64 decoded_index = 0; decoded_index < decoded.size; decoded_index++) {

    char lookup = decoded.data[decoded_index];
    bool found = false;
    code c;
    for (u64 i = 0; i < codes.size; i++) {
      if (lookup == codes.data[i].letter) {
        c = codes.data[i];
        found = true;
        break;
      }
    }
    panic_if(!found, "Could not find code when encoding.");

    for (u64 i = 0; i < c.length; i++) {
      current_byte |= ((c.bits >> i) & 1) << (bit_counter - 1);
      bit_counter--;
      if (bit_counter == 0) {
        assert(encoded_index < encoded.size);
        encoded.data[encoded_index] = current_byte;
        encoded_index++;
        bit_counter = 8;
        current_byte = 0;
      }
    }
  }
  if (encoded_index == encoded.size - 1) {// round off the last byte
    assert(bit_counter != 8);
    encoded.data[encoded_index] = current_byte;
    encoded_index++;
  }

  assert(encoded_index == encoded.size);
}

// needs some sort of header
inline void huffman_decode_using_binary_heap(const file& encoded, file& decoded) {
  constexpr u64 max_table_size = 256;
  local_array<letter_count, max_table_size> byte_counts;

  u64 *header = reinterpret_cast<u64*>(encoded.data);
  u64 num_bytes_decoded = 0;
  for (u64 i = 0; i < max_table_size; i++) {
    byte_counts.push_back(letter_count{header[i], (u8)i});
    num_bytes_decoded += header[i];
  }

  // copied from encode
  using tree_t = binary_tree<max_table_size * 2>;
  tree_t tree; // 256 * 2 - 1 is max needed, if all bytes are equally likely, +1 for a garbage node. so 256 * 2 in total.
  binary_heap<tree_t::node_ptr, max_table_size> heap;
  for (u64 i = 0; i < byte_counts.size; i++) {
    if (byte_counts.data[i].count > 0) {
      tree_t::node_ptr n = tree.new_node(tree_t::node_ptr{nullptr}, tree_t::node_ptr{nullptr}, byte_counts.data[i]);
      heap.insert(n);
    }
  }

  while (heap.top().p->count.count != num_bytes_decoded) { // lol looks awful.
    tree_t::node_ptr p1 = heap.remove();
    tree_t::node_ptr p2 = heap.remove();
    letter_count total_count = {p1.p->count.count + p2.p->count.count, 0};
    tree_t::node_ptr n = tree.new_node(p1, p2, total_count);
    heap.insert(n);
  }

  tree_t::node_ptr top_of_tree = heap.remove();
  assert(heap.num_inserted == 0);

  local_array codes = tree.get_codes(byte_counts, top_of_tree);
  u64 max_bit_length = 0;
  for (u64 i = 0; i < codes.length; i++) {
    if (codes.data[i].length > max_bit_length) max_bit_length = codes.data[i].length;
  }
  assert(compute_length_of_compressed_file(codes) + max_table_size * 8 == encoded.size);

  decoded.alloc(num_bytes_decoded);

  u64 current_code = 0;
  code correct_code = {0};

  u64 encoded_bit_offset = 0;
  for (u64 decoded_index = 0; decoded_index < decoded.size; decoded_index++) {
    bool found_code = false;
    for (u64 num_bits = 0; num_bits < max_bit_length; num_bits++) {

      u8 current_byte = encoded.data[max_table_size * 8 + encoded_bit_offset / 8];
      u8 remainder = encoded_bit_offset % 8;

      u8 current_bit = (current_byte >> (8 - remainder - 1)) & 1;
      current_code |= current_bit << num_bits;

      for (u64 i = 0; i < codes.length; i++) {
        if ((num_bits + 1) == codes.data[i].length && codes.data[i].bits == current_code) {
          found_code = true;
          correct_code = codes.data[i];
        }
      }

      encoded_bit_offset++;
      if (found_code) {
        decoded.data[decoded_index] = correct_code.letter;
        current_code = 0;
        break;
      }
    }

    panic_if(!found_code, "Unknown code in encoded file.");
  }
}

};

#endif

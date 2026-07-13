#include "compression.hpp"

#include <iostream>

using namespace compression;

int main() {
  file decoded = file("./data/input.txt");
  for (u64 i = 0; i < decoded.size; i++) {
    std::cout << decoded.data[i];
  }
  file encoded = file();

  huffman_byte_encode(decoded, encoded);
  return 0;
}

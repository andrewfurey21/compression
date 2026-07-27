#include "compression.hpp"

using namespace compression;

int main(int argc, char **argv) {
  // panic_if(argc != 2, "Takes input file");
  // file decoded = file("./data/test.txt");
  file decoded = file("./data/input.txt");
  // file decoded = file(argv[1]);
  decoded.read();
  file encoded = file("./data/compressed");
  huffman_encode_using_binary_heap(decoded, encoded);
  // then decode and compare with decoded.
  encoded.write();
  return 0;
}

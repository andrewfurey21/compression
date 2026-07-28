#include "compression.hpp"

using namespace compression;

int main(int argc, char **argv) {
  panic_if(argc != 2 && argc != 3, "Takes input file and optional output file.");

  file encoded = file(argv[1]);
  file decoded = file(argc == 2 ? "./decompressed" : argv[2]);
  encoded.read();

  huffman_decode_using_binary_heap(encoded, decoded);
  decoded.write();
  return 0;
}

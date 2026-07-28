#include "compression.hpp"

using namespace compression;

int main(int argc, char **argv) {
  panic_if(argc != 2 && argc != 3, "Takes input file and optional output file.");

  file decoded = file(argv[1]);
  file encoded = file(argc == 2 ? "./compressed" : argv[2]);
  decoded.read();

  huffman_encode_using_binary_heap(decoded, encoded);
  encoded.write();
  return 0;
}

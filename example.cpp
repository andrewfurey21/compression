#include "compression.hpp"

#include <iostream>

using namespace compression;

int main() {
  // file decoded = file("./data/input.txt");
  // file encoded = file();
  // huffman_byte_encode(decoded, encoded);

  binary_heap<u64, 10> heap;

  heap.insert(10);
  std::cout << heap.array << "\n";
  heap.insert(15);
  std::cout << heap.array << "\n";
  heap.insert(11);
  std::cout << heap.array << "\n";
  heap.insert(1);
  std::cout << heap.array << "\n";
  heap.insert(8);
  std::cout << heap.array << "\n";
  heap.insert(100);
  std::cout << heap.array << "\n";
  heap.insert(53);
  std::cout << heap.array << "\n";
  heap.insert(0);
  std::cout << heap.array << "\n";


  u64 popped = heap.remove();
  std::cout << popped << "\n";
  std::cout << heap.array << "\n";
  std::cout << std::endl;

  popped = heap.remove();
  std::cout << popped << "\n";
  std::cout << heap.array << "\n";
  std::cout << std::endl;

  popped = heap.remove();
  std::cout << popped << "\n";
  std::cout << heap.array << "\n";
  std::cout << std::endl;

  popped = heap.remove();
  std::cout << popped << "\n";
  std::cout << heap.array << "\n";
  std::cout << std::endl;

  popped = heap.remove();
  std::cout << popped << "\n";
  std::cout << heap.array << "\n";
  std::cout << std::endl;

  popped = heap.remove();
  std::cout << popped << "\n";
  std::cout << heap.array << "\n";
  std::cout << std::endl;

  popped = heap.remove();
  std::cout << popped << "\n";
  std::cout << heap.array << "\n";
  std::cout << std::endl;

  popped = heap.remove();
  std::cout << popped << "\n";
  std::cout << heap.array << "\n";
  std::cout << std::endl;


  heap.insert(10);
  std::cout << heap.array << "\n";
  heap.insert(15);
  std::cout << heap.array << "\n";
  heap.insert(11);
  std::cout << heap.array << "\n";
  heap.insert(1);
  std::cout << heap.array << "\n";
  heap.insert(8);
  std::cout << heap.array << "\n";
  heap.insert(100);
  std::cout << heap.array << "\n";
  heap.insert(53);
  std::cout << heap.array << "\n";
  heap.insert(0);
  std::cout << heap.array << "\n";


  popped = heap.remove();
  std::cout << popped << "\n";
  // std::cout << heap.array << "\n";
  std::cout << std::endl;

  popped = heap.remove();
  std::cout << popped << "\n";
  // std::cout << heap.array << "\n";
  std::cout << std::endl;

  popped = heap.remove();
  std::cout << popped << "\n";
  // std::cout << heap.array << "\n";
  std::cout << std::endl;

  popped = heap.remove();
  std::cout << popped << "\n";
  // std::cout << heap.array << "\n";
  std::cout << std::endl;

  popped = heap.remove();
  std::cout << popped << "\n";
  // std::cout << heap.array << "\n";
  std::cout << std::endl;

  popped = heap.remove();
  std::cout << popped << "\n";
  // std::cout << heap.array << "\n";
  std::cout << std::endl;

  popped = heap.remove();
  std::cout << popped << "\n";
  // std::cout << heap.array << "\n";
  std::cout << std::endl;

  popped = heap.remove();
  std::cout << popped << "\n";
  // std::cout << heap.array << "\n";
  std::cout << std::endl;



  return 0;
}

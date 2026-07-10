#include "compression.hpp"

#include <iostream>

using namespace compression;

int main() {
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

  std::cout << array << "\n";

  array.iterative_inplace_quicksort();

  std::cout << array << "\n";
  return 0;
}

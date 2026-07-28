all: compress decompress

compress: ./compress.cpp ./compression.hpp
	g++ compress.cpp -o compress -O3 -std=c++20

decompress: ./decompress.cpp ./compression.hpp
	g++ decompress.cpp -o decompress -O3 -std=c++20

clean:
	rm -rf example
.PHONY: clean

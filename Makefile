all: example

example: ./example.cpp ./compression.hpp
	g++ example.cpp -o example -g -std=c++20

clean:
	rm -rf example
.PHONY: clean

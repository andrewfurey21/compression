all: example

example: ./example.cpp ./compression.hpp
	g++ example.cpp -o example -g

clean:
	rm -rf example
.PHONY: clean

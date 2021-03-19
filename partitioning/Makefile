
runtests: test
	./test

test: test.cpp q.hpp qr.hpp leaf.hpp hbb.hpp base.hpp toposorting.hpp
	echo "Building $<" && g++ -std=c++17 -O0 -g -o test test.cpp


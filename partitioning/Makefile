
runtests: test test_indexing
	./test
	./test_indexing

test: test.cpp qr.hpp leaf.hpp hbb.hpp base.hpp
	echo "Building $<" && clang++ -std=c++17 -O0 -g -o test test.cpp

test_indexing: test_indexing.cpp qr.hpp leaf.hpp hbb.hpp base.hpp indexer.hpp
	echo "Building $<" && clang++ -std=c++17 -O0 -g -o test_indexing test_indexing.cpp



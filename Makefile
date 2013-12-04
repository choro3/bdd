bdd:	
	$(CXX) -I./src -o bdd src/main.cpp src/bdd.cpp
	$(CXX) -I./src -o test/bdd_test src/test.cpp src/bdd.cpp

test:	test/bdd_test
	./test/bdd_test < test/test.in | diff - test/test.out

clean:
	rm bdd test/bdd_test

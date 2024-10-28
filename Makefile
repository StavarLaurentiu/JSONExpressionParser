CXX = g++
CXXFLAGS = -std=c++11 -O2

all: json_eval permission_test

json_eval: json_eval.cpp
	$(CXX) $(CXXFLAGS) -o json_eval json_eval.cpp

permission_test: test.sh
	chmod +x test.sh

run_tests: json_eval permission_test
	./test.sh

clean:
	rm -f json_eval
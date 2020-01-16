default: test clean_o

CPP_FOLDER=../topwords

test: test.o topwords_lib.o
	g++ test.o topwords_lib.o -o test -std=c++17 -O2 -Wall -fopenmp

test.o: test.cpp ${CPP_FOLDER}/topwords_lib.h
	g++ -c test.cpp -o test.o -std=c++17 -O2 -Wall -fopenmp

topwords_lib.o: ${CPP_FOLDER}/topwords_lib.cpp ${CPP_FOLDER}/topwords_lib.h
	g++ -c ${CPP_FOLDER}/topwords_lib.cpp -o topwords_lib.o -std=c++17 -O2 -Wall -fopenmp

clean_o:
	rm *.o

clean:
	rm *.o main


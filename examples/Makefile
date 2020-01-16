default: main clean_o

CPP_FOLDER=../topwords

main: main.o topwords_lib.o
	g++ main.o topwords_lib.o -o main -std=c++17 -O2 -Wall -fopenmp

main.o: main.cpp ${CPP_FOLDER}/topwords_lib.h
	g++ -c main.cpp -o main.o -std=c++17 -O2 -Wall -fopenmp

topwords_lib.o: ${CPP_FOLDER}/topwords_lib.cpp ${CPP_FOLDER}/topwords_lib.h
	g++ -c ${CPP_FOLDER}/topwords_lib.cpp -o topwords_lib.o -std=c++17 -O2 -Wall -fopenmp

clean_o:
	rm *.o

clean:
	rm *.o main


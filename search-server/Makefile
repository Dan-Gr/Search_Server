FLAG = -Wall -Wextra -std=c++17 -ltbb -pthread 
FLAGS = -Wall -Wextra -Warray-bounds -g -fsanitize=address -std=c++17

all:    search_server

search_server:  document.o read_input_functions.o remove_duplicates.o search_server.o string_processing.o request_queue.o main.o
	g++ -o search_server document.o read_input_functions.o remove_duplicates.o search_server.o string_processing.o request_queue.o main.o -ltbb -pthread 
	rm rf *.o *.gch

main.o: main.cpp 
	g++ -c main.cpp $(FLAG)

document.o: document.cpp
	g++ -c document.cpp document.h $(FLAG)

read_input_functions.o: read_input_functions.cpp
	g++ -c read_input_functions.cpp read_input_functions.h $(FLAG)

request_queue.o:    request_queue.cpp
	g++ -c request_queue.cpp request_queue.h $(FLAG)

search_server.o:    search_server.cpp
	g++ -c search_server.cpp search_server.h log_duration.h remove_duplicates.h $(FLAG)

string_processing.o:    string_processing.cpp
	g++ -c string_processing.cpp string_processing.h $(FLAG)

remove_duplicates.o:	remove_duplicates.cpp
	g++ -c remove_duplicates.cpp remove_duplicates.h $(FLAG)

process_queries.o:	process_queries.cpp
	g++ -c process_queries.cpp process_queries.h $(FLAG)
clean: 
	rm rf *.o *.gch
	rm rf main
	rm rf *.exe

reb: clean search_server

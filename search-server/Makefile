FLAG = -Wall -Wextra -Warray-bounds -std=c++17
FLAGS = -Wall -Wextra -Warray-bounds -g -fsanitize=address -std=c++17

all:    search_server

search_server:  document.o read_input_functions.o remove_duplicates.o search_server.o string_processing.o request_queue.o main.o
	g++ document.o read_input_functions.o remove_duplicates.o search_server.o string_processing.o request_queue.o main.o -o search_server
	del *.o *.gch

main.o: main.cpp 
	g++ -c $(FLAG) main.cpp

document.o: document.cpp
	g++ -c $(FLAG) document.cpp document.h

read_input_functions.o: read_input_functions.cpp
	g++ -c $(FLAG) read_input_functions.cpp read_input_functions.h

request_queue.o:    request_queue.cpp
	g++ -c $(FLAG) request_queue.cpp request_queue.h

search_server.o:    search_server.cpp
	g++ -c $(FLAG) search_server.cpp search_server.h log_duration.h remove_duplicates.h

string_processing.o:    string_processing.cpp
	g++ -c $(FLAG) string_processing.cpp string_processing.h

remove_duplicates.o:	remove_duplicates.cpp
	g++ -c $(FLAG) remove_duplicates.cpp remove_duplicates.h

clean: 
	del *.o *.gch
	del main
	del *.exe

reb: clean search_server

FLAG = -Wall -g -Wextra -Wmaybe-uninitialized -Wfatal-errors

all:    search_server

search_server:  main.o document.o read_input_functions.o request_queue.o search_server.o string_processing.o
	g++ main.o document.o read_input_functions.o request_queue.o search_server.o string_processing.o -o search_server
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
	g++ -c $(FLAG) search_server.cpp search_server.h

string_processing.o:    string_processing.cpp
	g++ -c $(FLAG) string_processing.cpp string_processing.h

clean: 
	del *.o *.gch
	del search_server.exe
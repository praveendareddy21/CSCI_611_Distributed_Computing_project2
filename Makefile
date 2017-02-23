all:test_main auto_test stub

test_main: test_main.cpp libmap.a goldchase.h
	g++ test_main.cpp -o test_main -L. -lpthread -lmap -lpanel -lncurses -lrt

libmap.a: Screen.o Map.o
	ar -r libmap.a Screen.o Map.o

Map.o: Map.cpp Map.h
	g++ -c Map.cpp

auto_test: auto_test.cpp goldchase.h
	g++ auto_test.cpp -o auto_test  -lpthread  -lrt
	
stub: stub.cpp goldchase.h Map.o
	g++ stub.cpp Map.o -o stub  -lpthread  -lrt
	
clean:
	rm -f Screen.o Map.o libmap.a test_main auto_test stub

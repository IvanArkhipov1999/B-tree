all: a

a: Main.o Bplustree.o 
	g++ -g Main.o Bplustree.o -o a

Main.o: Main.cpp
	g++ -c Main.cpp -g
Bplustree.o: Bplustree.cpp
	g++ -c Bplustree.cpp -g  






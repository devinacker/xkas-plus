clear
g++ -std=gnu++0x -I. -c xkas.cpp
g++ -std=gnu++0x -I. -c libxkas/libxkas.cpp
g++ -o xkas xkas.o libxkas.o
strip -s xkas
rm *.o

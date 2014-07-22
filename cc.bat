mingw32-g++ -std=gnu++0x -I. -c xkas.cpp

mingw32-g++ -std=gnu++0x -I. -c libxkas/libxkas.cpp

mingw32-g++ -o xkas xkas.o libxkas.o

strip -s xkas.exe

@del *.o

@pause

CP=g++
CFLAGS=-std=c++11 -Wall -Wextra -pedantic -g 


csim: csim.o 
	$(CP) csim.o -o csim

csim.o: csim.cpp 
	$(CP) $(CFLAGS) -c csim.cpp 

clean:
	rm -f *.o csim 
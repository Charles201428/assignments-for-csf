CP=g++
CFLAGS=-std=c++11 -Wall -Wextra -pedantic -g 


csim: csim.o 
	$(CP) csim.o -o csim

csim.o: csim.cpp 
	$(CP) $(CFLAGS) -c csim.cpp 

# Use this target to create a zipfile that you can submit to Gradescope
.PHONY: solution.zip
solution.zip :
	rm -f solution.zip
	zip -9r $@ Makefile README.txt *.h *.cpp

clean:
	rm -f *.o csim 
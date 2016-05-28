CXXFLAGS = -std=c++0x -save-temps
CC = g++
SRCS = $(wildcard *.cpp) 
OBJS = $(patsubst %.cpp,%.o,$(SRCS))
HEADERS = $(wildcard *.hpp) 
 
main: $(OBJS) 

-include .depend

.depend: $(SRCS) $(HEADERS)
	$(CC) $(CXXFLAGS) -MM $(SRCS) > .depend

clean:
	-rm *.ii
	-rm *.i
	-rm *.s
	-rm *.o
	-rm *.h.gch

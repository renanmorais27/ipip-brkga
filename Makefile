# Macros:
CPP= g++
CFLAGS= -O3 -fopenmp
OBJECTS= IPIPInstance.o IPIPSolver.o IPIPDecoder.o brkga-ipip.o

# Targets:
all: brkga-ipip

brkga-ipip: $(OBJECTS)
	$(CPP) $(CFLAGS) $(OBJECTS) -lpython3.7m -o brkga-ipip
        
brkga-ipip.o:
	$(CPP) $(CFLAGS) -c brkga-ipip.cpp
	
IPIPInstance.o:
	$(CPP) $(CFLAGS) -c IPIPInstance.cpp	

IPIPSolver.o:
	$(CPP) $(CFLAGS) -c IPIPSolver.cpp

IPIPDecoder.o:
	$(CPP) $(CFLAGS) -c IPIPDecoder.cpp

# Remove:
clean:
	rm -f ipipdecoder $(OBJECTS)

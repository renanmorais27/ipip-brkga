# Macros:
CPP= g++
CFLAGS= -O3 -fopenmp
OBJECTS= IPIPDecoder.o brkga-ipip.o

# Targets:
all: brkga-ipip

brkga-ipip: $(OBJECTS)
	$(CPP) $(CFLAGS) $(OBJECTS) -lpython3.7m -o brkga-ipip
        
brkga-ipip.o:
	$(CPP) $(CFLAGS) -c brkga-ipip.cpp
	
IPIPDecoder.o:
	$(CPP) $(CFLAGS) -c IPIPDecoder.cpp

# Remove:
clean:
	rm -f ipipdecoder $(OBJECTS)

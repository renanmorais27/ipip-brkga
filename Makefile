# Macros:
CPP= g++
CFLAGS= -O3 -fopenmp
OBJECTS= SampleDecoder.o samplecode.o

# Targets:
all: samplecode

samplecode: $(OBJECTS)
	$(CPP) $(CFLAGS) $(OBJECTS) -lpython3.7m -o samplecode
        
samplecode.o:
	$(CPP) $(CFLAGS) -c samplecode.cpp
	
SampleDecoder.o:
	$(CPP) $(CFLAGS) -c SampleDecoder.cpp

# Remove:
clean:
	rm -f sampledecoder $(OBJECTS)

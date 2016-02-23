CC=g++
R_LDFLAGS =     `root-config --ldflags`
R_LIBS    =     `root-config --libs`
R_CFLAGS  =     `root-config --cflags`
R_ALL     =     $(R_LADFLAGS) $(R_LIBS) $(R_CFLAGS)
DEPS = ResonanceSimulator.h

%.o: %.C $(DEPS)
	$(CC) -Wall -fPIC $(R_ALL) -c -o $@ $< 

ResonanceFinder: ResonanceFinder.o ResonanceSimulator.o
	$(CC) -Wall -fPIC $(R_ALL) -c -o ResonanceSimulator.o ResonanceSimulator.C
	$(CC) -Wall -fPIC $(R_ALL) -c -o ResonanceFinder.o ResonanceFinder.C
	$(CC) -ansi -W -Wall $(R_ALL) -lGui -o ResonanceFinder ResonanceFinder.o ResonanceSimulator.o

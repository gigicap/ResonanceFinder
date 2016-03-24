CC=g++

R_LDFLAGS =     `root-config --ldflags`
R_LIBS    =     `root-config --libs`
R_CFLAGS  =     `root-config --cflags`

XERCES_LIBS  = 	-lxerces-c
ROOFIT_LIBS =  -lRooFit -lRooFitCore

R_ALL     =     $(R_LADFLAGS) $(R_LIBS) $(ROOFIT_LIBS) $(R_CFLAGS) $(XERCES_LIBS)
DEPS = ResonanceSimulator.h ReadConfig.h

%.o: %.C $(DEPS)
	$(CC) -Wall -fPIC $(R_ALL) -c -o $@ $< 

ResonanceFinder: ReadConfig.o ResonanceFinder.o ResonanceSimulator.o 
	$(CC) -Wall -fPIC $(R_ALL) -c -o ReadConfig.o ReadConfig.cc 
	$(CC) -Wall -fPIC $(R_ALL) -c -o ResonanceSimulator.o ResonanceSimulator.C 
	$(CC) -Wall -fPIC $(R_ALL) -c -o ResonanceFinder.o ResonanceFinder.C
	$(CC) -ansi -W -Wall $(R_ALL) -lGui -o ResonanceFinder ResonanceFinder.o ResonanceSimulator.o ReadConfig.o

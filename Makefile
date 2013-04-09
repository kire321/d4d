CC=g++
CFLAGS=-g -Wall -O3

default: analysis
 #	predictor

analysis: main.o \
	user.o \
	antenna.o
		$(CC) -o $@ $^ $(CFLAGS)

main.o: user.h \
	antenna.h

# %.o: %.cpp %.h
# 	$(CC) -c $< -o $@ $(CFLAGS)

antenna.o: antenna.h
# multiDimVala.o: multiDimVala.h
user.o: user.h

predictor: predictor.o
		$(CC) -o $@ $^ $(LIBS) $(CFLAGS)

clean:
	rm -f predictor analysis *.o

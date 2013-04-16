CC=g++
CFLAGS=-g -Wall -O3

default: predictor
	#	analysis

analysis: main.o \
	user.o \
	antenna.o
		$(CC) -o $@ $^ $(CFLAGS)

main.o: user.h \
	antenna.h

# %.o: %.cpp %.h
# 	$(CC) -c $< -o $@ $(CFLAGS)

antenna.o: antenna.h
user.o: user.h
antenna_model.o: antenna_model.h user.h globals.h
user_model.o: user_model.h
table.o: table.h
path.o: path.h antenna.h antenna_model.h

predictor: predictor.o antenna.o user.o antenna_model.o user_model.o table.o \
	path.o
		$(CC) -o $@ $^ $(LIBS) $(CFLAGS)

predictor.o: antenna_model.h user_model.h

clean:
	rm -f predictor analysis *.o

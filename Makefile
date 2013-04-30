CC=g++
CFLAGS=-g -Wall -O3 -std=c++0x

default: predictor
	#	analysis

analysis: main.o \
	user.o \
	antenna.o
		$(CC) $(CFLAGS) -o $@ $^

run:
	./predictor ../data/ANT_POS.TSV < ../data/events/sorted0.TSV  > ../output/output.json 2> ../output/err.txt

predictor: predictor.o \
	antenna.o \
	user.o \
	antenna_model.o \
	user_model.o \
	path.o
		$(CC) $(CFLAGS) -o $@ $^

main.o: user.h \
	antenna.h

# %.o: %.cpp %.h

antenna.o: antenna.cpp antenna.h types.h multiDimVala.h
	$(CC) -c $< -o $@ $(CFLAGS)
user.o: user.cpp user.h antenna_model.h types.h multiDimVala.h path.h
	$(CC) -c $< -o $@ $(CFLAGS)
antenna_model.o: antenna_model.cpp antenna_model.h utils.h user_model.h user.h antenna.h path.h types.h
	$(CC) -c $< -o $@ $(CFLAGS)
user_model.o: user_model.cpp user_model.h user.h types.h
	$(CC) -c $< -o $@ $(CFLAGS)
path.o: path.cpp path.h antenna.h antenna_model.h types.h
	$(CC) -c $< -o $@ $(CFLAGS)
predictor.o: predictor.cpp types.h utils.h antenna_model.h user_model.h
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -f predictor analysis *.o

all: testmidi

testmidi: testmidi.o MidiFile.o
	g++ -o testmidi testmidi.o MidiFile.o

testmidi.o: testmidi.cpp ../MidiFile.h
	g++ -std=c++17 -c -g testmidi.cpp

MidiFile.o: ../MidiFile.cpp ../MidiFile.h
	g++ -std=c++17 -c -g ../MidiFile.cpp

clean:
	rm -vf core
	rm -vf *.o
	rm -vf testmidi


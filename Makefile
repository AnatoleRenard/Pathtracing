CFLAGS = -O2
LDFLAGS = -lassimp -lSDL2 -ldraco

out:  *.cpp src/*.cpp  src/*.hpp
	g++ -o main *.cpp src/*.cpp $(LDFLAGS) $(CFLAGS)

.PHONY: test clean

test: main
	./main

log: main
	./main -> log.txt

clean:
	rm -f main

FILE = main
CC = g++
CFLAGS = -framework OpenGL -framework GLUT -Wno-deprecated-declarations -std=c++11
EXE = perlinNoise

all: $(EXE)

$(EXE): $(FILE).cpp
	$(CC) $(FILE).cpp -o $(EXE) $(CFLAGS)

# deletes all object files and executable
clean:
	rm -f *.o $(EXE)


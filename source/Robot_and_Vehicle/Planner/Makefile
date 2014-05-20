CC=gcc
CFLAGS=-I.
DEPS = shmemory.h planner.h
OBJ = shmemory.o planner.o main.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

clean:
	rm -f $(OBJ) planner main *.o *.a

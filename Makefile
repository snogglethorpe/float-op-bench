CC = gcc-snapshot
CFLAGS = -O3 -ffast-math -march=native
LDLIBS = -lm

BENCHS = float-op-bench exp-bench

all: $(BENCHS)

clean:
	$(RM) $(BENCHS) *.o

float-op-bench: float-op-bench.o glibc-x86_64-expf.o glibc-generic-expf.o glibc-generic-exp.o

exp-bench: exp-bench.o

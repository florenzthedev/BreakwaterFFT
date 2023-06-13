#  Copyright (c) 2023 Zachary Todd Edwards
#  MIT License

#To use clang: add -cc=clang to CC and remove -fcx-limited-range from CFLAGS
CC = mpicc 
CFLAGS = -Wall -I$(HEDDIR) -fcx-limited-range

SRCDIR = ./src
HEDDIR = ./include
OBJDIR = ./obj
TSTDIR = ./tests

EXEC = breakwater

LIBS = -lm

_DEPS = bitmanip.h fft.h logging.h messaging.h node.h options.h
DEPS = $(patsubst %,$(HEDDIR)/%,$(_DEPS))

_OBJ =  fft.o logging.o main.o messaging.o node.o options.o
OBJ = $(patsubst %,$(OBJDIR)/%,$(_OBJ))

$(EXEC): $(OBJ)
	$(CC) -o $@ $(OBJ) $(LIBS) $(CFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(DEPS) | $(OBJDIR)
	$(CC) -c -o $@ $< $(CFLAGS)

$(OBJDIR):
	mkdir -p $@

.PHONY: clean

debug: CFLAGS += -g -D_DEBUG
debug: clean $(EXEC)
	@echo ----  Launching Test 1 With 2 Nodes  ----
	mpiexec -n 3 ./$(EXEC) $(TSTDIR)/test1.csv
	$(MAKE) clean

test: $(EXEC)
	@echo ----  TEST 1  ----
	mpiexec -n 4 ./$(EXEC) $(TSTDIR)/test1.csv
	@echo ----  TEST 2  ----
	mpiexec -n 4 ./$(EXEC) $(TSTDIR)/test2.csv

clean:
	rm -f $(OBJDIR)/*.o core

SDIR=./source
IDIR=./include
CC=gcc
CFLAGS=-I$(IDIR)

ODIR=./artifacts/obj
LDIR=./artifacts/lib
ADIR=./artifacts/kingLouis.exe

LIBS=-lm

_DEPS = drawBoard.h gameState.h pieces.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o drawBoard.o 
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

kingLouis: $(OBJ)
	$(CC) -o $(ADIR) $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
CC = gcc
LD = gcc
LDFLAGS = -Wall -O2 -std=c99
EXES = lpcrc padto

all: $(EXES)

% : %.c
	$(LD) $(LDFLAGS) -o $@ $<

clean: 
	rm -f $(EXES)

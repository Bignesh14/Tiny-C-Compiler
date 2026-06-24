CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -g
SRC     = main.c lex.c parser.c semcheck.c codegen.c
OBJ     = $(SRC:.c=.o)
TARGET  = tcc

all: $(TARGET)
	
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJ) $(TARGET) $(TARGET).exe

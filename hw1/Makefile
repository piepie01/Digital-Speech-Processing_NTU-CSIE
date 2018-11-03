.PHONY: all clean

CFLAGS+=
LDFLAGS+=-lm     # link to math library
GCC = g++
TARGET=train
TEST=test

all: $(TARGET)
	$(GCC) $(TARGET).cpp -g -Wall -o $(TARGET)
	$(GCC) $(TEST).cpp -g -Wall -o $(TEST)

clean:
	$(RM) $(TARGET)
	$(RM) $(TEST)

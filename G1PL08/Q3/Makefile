CC = gcc
CFLAGS = -Wall
TARGET = tokenring

all: $(TARGET)

$(TARGET): $(TARGET).c
	@ mkdir pipes
	@ $(CC) $(CFLAGS) $(TARGET).c -o $(TARGET) -lm

clean:
	@ $(RM) $(TARGET)
	@ $(RM) -r pipes
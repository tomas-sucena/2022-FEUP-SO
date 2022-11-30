CC = gcc
CFLAGS = -Wall
TARGET = samples

all: $(TARGET)

$(TARGET): $(TARGET).c
	@ $(CC) $(CFLAGS) $(TARGET).c -o $(TARGET)

clean:
	@ $(RM) $(TARGET)
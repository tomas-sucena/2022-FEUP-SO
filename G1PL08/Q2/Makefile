CC = gcc
CFLAGS = -Wall
TARGET = txt2epub2

all: $(TARGET)

$(TARGET): $(TARGET).c
	@ $(CC) $(CFLAGS) $(TARGET).c -o $(TARGET)

clean:
	@ $(RM) $(TARGET)
	@ $(RM) *.txt
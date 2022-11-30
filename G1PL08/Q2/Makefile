CC = gcc
CFLAGS = -Wall
TARGET = txt2epub

all: $(TARGET)

$(TARGET): $(TARGET).c
	@ $(CC) $(CFLAGS) $(TARGET).c -o $(TARGET)

clean:
	@ $(RM) $(TARGET)
	@ $(RM) *.txt
	@ $(RM) *.epub
	@ $(RM) *.zip
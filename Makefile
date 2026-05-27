CC     = gcc
CFLAGS = -Wall -Wextra
TARGET = aeroportos
SRCS   = main.c grafo_aeroportos.c matriz_esparsa.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)

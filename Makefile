SRCS=$(wildcard ./src/*.c)
INCLUDES=./inc
CFLAGS = -Wall -I$(INCLUDES)

OBJS=$(SRCS:.c=.o)

TARGET=BebopVideoStreamer

$(TARGET): $(SRCS)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	rm *.o $(TARGET)

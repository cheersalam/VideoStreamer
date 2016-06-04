SRCS=$(wildcard *.c)
CFLAGS = -Wall

OBJS=$(SRCS:.c=.o)

TARGET=BebopVideoStreamer

$(TARGET): $(SRCS)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	rm *.o $(TARGET)

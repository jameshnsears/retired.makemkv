CXXFLAGS = -g -Wall -fmessage-length=0

OBJS =		key.o

LIBS =

TARGET =	key

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

CC = clang++
CXXFLAGS = -std=c++17
CFLAGS = -pthreads -Wall -g
INCLUDE = -Iinclude -I/usr/local/include
INCLUDE_DIR = -Iinclude
#REDIS = /usr/local/lib/libredis++.a /usr/include/hiredis
REDIS = -lredis++ -lhiredis
LDFLAGS = -L/usr/local/lib

DIR = src
TARGET = app
OBJFILES = main.o processor.o generator.o data.o

all: $(TARGET)
	rm $(OBJFILES)

$(TARGET): $(OBJFILES)
	$(CC) $(CXXFLAGS) $(INCLUDE) $(LDFLAGS) $(CFLAGS) -o $(TARGET).exe $(REDIS) $(OBJFILES)
	
%.o: $(DIR)/%.cpp
	$(CC) $(CXXFLAGS) $(INCLUDE_DIR) -c $^

clean:
	rm -f *.exe *.o *.txt
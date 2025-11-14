
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -O2 -g
TARGET = simple_shell
SOURCES = main.cpp shell.cpp builtins.cpp parser.cpp executor.cpp
HEADERS = shell.hpp
OBJS = $(SOURCES:.cpp=.o)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

debug: CXXFLAGS += -DDEBUG -g
debug: $(TARGET)

clean:
	rm -f $(TARGET) $(OBJS)

install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

.PHONY: clean install debug
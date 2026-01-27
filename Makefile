
CXX := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -I include

SRCS := src/main.cpp src/loadbalancer.cpp src/webserver.cpp src/request.cpp
OBJS := $(SRCS:.cpp=.o)
TARGET := lb_sim

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

run: $(TARGET)
	./$(TARGET)

test: 
	$(CXX) $(CXXFLAGS) -o test_runner tests/test_basic.cpp src/loadbalancer.cpp src/webserver.cpp src/request.cpp
	./test_runner

clean:
	rm -f $(OBJS) $(TARGET)

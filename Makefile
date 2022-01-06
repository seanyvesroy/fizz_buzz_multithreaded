CXXFLAGS = -g -std=c++17 -Wall -Wextra -Wpedantic -Werror -pthread -fsanitize=thread
SRCS = Producer-Consumer.cpp \
       Ordering-Output.cpp \
       Multiple-Producers.cpp \
       Multiple-Consumers.cpp
       
TARGETS = $(SRCS:.cpp=)

.PHONY: clean

all: $(TARGETS)

$(TARGETS): % : %.o pthread-wrappers.o
	$(CXX) $(CXXFLAGS) -o $@ $^

pthread-wrappers.o: pthread-wrappers.cpp pthread-wrappers.h
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f *.o $(TARGETS)

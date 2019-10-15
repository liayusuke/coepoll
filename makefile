CC=g++

CFLAGS= -g

INCLUDE=-I ../myboost/include

LIBRARY=-L ../myboost/lib\
		-l:libboost_context.a

BIN=coepoll

OBJ= example.o epoll.o coroutine.o co_udp.o co_tcp.o util.o entry.o

$(BIN): $(OBJ)
	$(CC) -o $@ $^ $(LIBRARY)

$(OBJ): %.o: %.cpp
	$(CC) -c $(CFLAGS) $(INCLUDE) $< -o $@

clean:
	rm -f $(OBJ) $(BIN)

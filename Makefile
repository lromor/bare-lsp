CXX=g++
CXXFLAGS=-std=c++17 -O3 -W -Wall -Wextra -Wno-unused-parameter
LDFLAGS=-labsl_strings -labsl_status -labsl_throw_delegate
SCHEMA_COMPILER=../jcxxgen/jcxxgen

lsp-server: main.o
	$(CXX) -o $@ $^ $(LDFLAGS)

main.o: main.cc lsp-protocol.h json-rpc-server.h lsp-protocol.h message-stream-splitter.h lsp-text-buffer.h

main.cc:

json-rpc-server.h: lsp-protocol.h

lsp-protocol.h: lsp-protocol.yaml

%.h : %.yaml $(SCHEMA_COMPILER)
	$(SCHEMA_COMPILER) $< -o $@

clean:
	rm -f main.o lsp-server lsp-protocol.h

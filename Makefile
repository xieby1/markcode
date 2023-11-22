all: markcode

CFLAGS += -MMD
-include $(patsubst %.c,%.d,$(wildcard *.c))

LDFLAGS += -lpcre2-8 -ltree-sitter ${TREESITTER_PARSERS}

markcode: markcode.o re.o types.o

clean:
	rm -f *.o
	rm -f *.d
	rm -f markcode

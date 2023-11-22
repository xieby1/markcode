all: nix

CFLAGS += -MMD
-include $(patsubst %.c,%.d,$(wildcard *.c))

LDFLAGS += -lpcre2-8 -ltree-sitter ${TREESITTER_PARSERS}

nix: nix.o re.o types.o

clean:
	rm *.o
	rm *.d
	rm nix

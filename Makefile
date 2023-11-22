all: nix

LDFLAGS += -lpcre2-8 -ltree-sitter ${TREESITTER_PARSERS}

nix: nix.o re.o

clean:
	rm *.o
	rm nix

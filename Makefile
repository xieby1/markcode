all: bin/markcode
bin/markcode: src/markcode
	mkdir -p bin/
	cp $< $@

CFLAGS += -MMD -Isrc/include
LDFLAGS += -lpcre2-8 -ltree-sitter ${TREESITTER_PARSERS}
-include $(patsubst %.c,%.d,$(shell find src/ -name "*.c"))

src/markcode: src/markcode.o src/re.o src/types.o src/context.o

clean:
	rm -f src/*.o
	rm -f src/*.d
	rm -f src/markcode
	rm -rf bin/

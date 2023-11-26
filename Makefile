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

TESTS_CORRECT_MDs = $(shell find tests/ -name "*.correct.md")
TESTS_CHECK_MDs = $(subst correct.md,check.md,${TESTS_CORRECT_MDs})
TESTS_CHECKs = $(subst correct.md,check,${TESTS_CORRECT_MDs})
.INTERMEDIATE: ${TESTS_CHECK_MDs}
tests/%.check.md: src/markcode tests/%
	$^ > $@
tests/%.check: tests/%.check.md tests/%.correct.md
	diff $^
tests: ${TESTS_CHECKs}

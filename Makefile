.PHONY: all make_all make_gcc clean_gcc

all: make_gcc

# GCC (For more modern platforms)
make_gcc:
	$(MAKE) -f makefile.gcc

clean_gcc:
	$(MAKE) -f makefile.gcc clean

# TODO: N64 support

# Global clean
clean: clean_gcc
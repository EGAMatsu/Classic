.PHONY: all make_all make_gcc clean_gcc make_dummy clean_dummy make_dx9 clean_dx9

all: make_gcc

# GCC (For more modern platforms)
make_gcc:
	$(MAKE) -f makefile.gcc

clean_gcc:
	$(MAKE) -f makefile.gcc clean

# Dummy (Test platform)
make_dummy:
	$(MAKE) -f makefile.dmy

clean_dummy:
	$(MAKE) -f makefile.dmy clean

# DirectX 9
make_dx9:
	$(MAKE) -f makefile.dx9

clean_dx9:
	$(MAKE) -f makefile.dx9 clean

# TODO: N64 support

# Global clean
clean: clean_gcc clean_dummy clean_dx9
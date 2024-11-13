.PHONY: all FsLib SDLLib biggestDump clean

all: FsLib SDLLib biggestDump

# I'm only interested in build the libraries, not the test/examples.
FsLib:
	$(MAKE) -C FsLib/Switch/FsLib

SDLLib:
	$(MAKE) -C SDLLib/SDL

biggestDump: FsLib SDLLib
	$(MAKE) -C biggestDump send

clean:
	$(MAKE) -C FsLib clean
	$(MAKE) -C SDLLib clean
	$(MAKE) -C biggestDump clean

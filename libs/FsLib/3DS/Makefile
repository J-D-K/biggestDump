.PHONY: all FsLib TestApp Examples clean

all: FsLib TestApp Examples

FsLib:
	$(MAKE) -C FsLib

TestApp: FsLib
	$(MAKE) -C TestApp

Examples: FsLib
	$(MAKE) -C Examples

clean:
	$(MAKE) -C FsLib clean
	$(MAKE) -C TestApp clean
	$(MAKE) -C Examples clean

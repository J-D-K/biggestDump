.PHONY:	all SDL TestApp clean

all: SDL TestApp

SDL:
	$(MAKE) -C SDL

TestApp: SDL
	$(MAKE) -C TestApp

clean:
	$(MAKE) -C SDL clean
	$(MAKE) -C TestApp clean

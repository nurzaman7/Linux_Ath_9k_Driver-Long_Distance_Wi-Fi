.SUFFIXES: .tar.gz .c

override CFLAGS += -Wall -O0 -g
sendraw:sendraw.c

all: sendraw sendraw.tar.gz

%.tar.gz: DIR=$(subst .tar.gz,,$@)
%.tar.gz: %.c
	@mkdir -p ./$(DIR)-0.1
	@cp $^ Makefile ./$(DIR)-0.1
	tar -cz -f $@ ./$(DIR)-0.1


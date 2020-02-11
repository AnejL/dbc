SHELL = /bin/sh
CFLAGS = -lX11 -lasound -lpthread
CC = gcc

LINKLOC = /usr/local/bin/dbc
REFBARLOC = /usr/local/bin/refbar
PWD = $(shell pwd)

.SUFFIXES: 
.SUFFIXES: .sh .c .o 

install: db.c
	@if [ ! -f $(PWD)/config.h ]; then\
		cp config.def.h config.h;\
	fi\

	$(CC) $(CFLAGS) -o dbc -O1 db.c

	ln -sf $(PWD)/dbc $(LINKLOC)
	ln -sf $(PWD)/refbar.sh $(REFBARLOC)
clean:
	rm -f dbc
uninstall:
	rm -f $(LINKLOC) $(REFBARLOC)

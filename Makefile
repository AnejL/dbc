SHELL = /bin/sh
CFLAGS = -g -lX11 -lasound -lpthread -lxkbfile
OFLAG = -O2
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

	$(CC) db.c -L/usr/lib/i386-linux-gnu $(CFLAGS) $(OFLAG) -o dbc

	ln -sf $(PWD)/dbc $(LINKLOC)
	ln -sf $(PWD)/refbar.sh $(REFBARLOC)
clean:
	rm -f dbc
uninstall:
	rm -f $(LINKLOC) $(REFBARLOC)

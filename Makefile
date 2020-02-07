LINKLOC = /usr/local/bin/dbc
REFBARLOC = /usr/local/bin/refbar
PWD = $(shell pwd)

install: db.c
	gcc -lX11 -lasound -lpthread -o dbc -O1 db.c
	cp config.def.h config.h
	ln -s $(PWD)/dbc $(LINKLOC)
	ln -s $(PWD)/refbar.sh $(REFBARLOC)
clean:
	unlink $(LINKLOC)
	unlink $(REFBARLOC)
	rm dbc

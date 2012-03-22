CFLAGS ?= -pedantic -Wall -Wextra -O2
PREFIX ?= /usr/local

pgctl: pgctl.c
	${CC} -o $@ ${CFLAGS} $< -lparapin
	strip $@

install: pgctl
	install -m 4755 pgctl ${PREFIX}/bin/pgctl

uninstall:
	rm -f ${PREFIX}/bin/pgctl

clean:
	rm -f pgctl

.PHONY: clean install uninstall

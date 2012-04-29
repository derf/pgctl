CFLAGS ?= -pedantic -Wall -Wextra -O2
PREFIX ?= /usr/local

pgctl: pgctl.c
	${CC} -o $@ ${CFLAGS} $< -lparapin
	strip $@

install: pgctl
	install -m 4755 pgctl ${PREFIX}/bin/pgctl
	install -m  755 fadedown ${PREFIX}/bin/pgctl_fadedown
	install -m  755 fadeup ${PREFIX}/bin/pgctl_fadeup
	install -m  755 on ${PREFIX}/bin/pgctl_on
	install -m  755 off ${PREFIX}/bin/pgctl_off

uninstall:
	rm -f ${PREFIX}/bin/pgctl

clean:
	rm -f pgctl

.PHONY: clean install uninstall

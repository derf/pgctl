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
	install -m  755 mains ${PREFIX}/bin/pgctl_mains
	install -m  755 light ${PREFIX}/bin/pgctl_light
	install -m  755 wait ${PREFIX}/bin/pgctl_wait
	install -m  755 relax ${PREFIX}/bin/pgctl_relax
	install -m  755 rrelax ${PREFIX}/bin/pgctl_rrelax
	install -m  755 unrelax ${PREFIX}/bin/pgctl_unrelax
	install -m  755 afk ${PREFIX}/bin/pgctl_afk
	install -m  755 re ${PREFIX}/bin/pgctl_re

uninstall:
	rm -f ${PREFIX}/bin/pgctl

clean:
	rm -f pgctl

.PHONY: clean install uninstall

/*
 * Copyright © 2012 by Daniel Friesel <derf@finalrewind.org>
 * License: WTFPL:
 *   0. You just DO WHAT THE FUCK YOU WANT TO.
 */

#include <stdlib.h>
#include <parapin.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

/*
 * Assumes that the AVR optocoupler is connected between GND and pin 14.
 */

#define AVR_PIN LP_PIN02

#define PATH_MAINS "/tmp/.pgctl_mains"
#define PATH_LIGHT "/tmp/.pgctl_light"

static void save_state(int index)
{
	FILE *fh;
	const char *states[] = {
		"", "on", "off", "on", "off",
		"10p", "20p", "40p", "60p", "strobo"
	};

	umask(S_IWGRP | S_IWOTH);

	if (index <= 2)
		fh = fopen(PATH_MAINS ".new", "w");
	else
		fh = fopen(PATH_LIGHT ".new", "w");

	if (fh == NULL)
		perror("fopen");

	fputs(states[index], fh);

	if (fclose(fh) != 0)
		perror("fclose");

	if (index <= 2)
		rename(PATH_MAINS ".new", PATH_MAINS);
	else
		rename(PATH_LIGHT ".nem", PATH_LIGHT);
}

int main(int argc, char **argv)
{
	const char *commands[] = {
		"none", "mains_on", "mains_off", "light_on", "light_off",
		"light_10p", "light_20p", "light_40p", "light_60p",
		"light_strobe"
	};

	if (argc < 2)
		return 1;

	if (nice(-20) == -1)
		fputs("warning: unable to renice\n", stderr);

	if (pin_init_user(LPT1) < 0)
		return 1;

	pin_output_mode(AVR_PIN);
	set_pin(AVR_PIN);
	usleep(500000);

	for (int cmd = 0; cmd < 10; cmd++) {
		if (!strcmp(argv[1], commands[cmd])) {
			save_state(cmd);
			for (int i = -3; i < cmd; i++) {
				usleep(4000);
				set_pin(AVR_PIN);
				usleep(4000);
				clear_pin(AVR_PIN);
			}
			usleep(500000);
			return 0;
		}
	}

	return 1;
}

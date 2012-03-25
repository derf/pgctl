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

/*
 * Assumes that the AVR optocoupler is connected between GND and pin 14.
 */

#define AVR_PIN LP_PIN14

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

	for (int cmd = 0; cmd < sizeof(commands); cmd++) {
		if (!strcmp(argv[1], commands[cmd])) {
			for (int i = -3; i < cmd; i++) {
				usleep(5000);
				set_pin(AVR_PIN);
				usleep(5000);
				clear_pin(AVR_PIN);
			}
			usleep(500000);
			return 0;
		}
	}

	return 1;
}

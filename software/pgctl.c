/*
 * Copyright © 2012 by Daniel Friesel <derf@finalrewind.org>
 * License: WTFPL:
 *   0. You just DO WHAT THE FUCK YOU WANT TO.
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>

/**
 * ioprio is not yet exported by glibc
 */

#define IOPRIO_BITS      (16)
#define IOPRIO_CLASS_SHIFT    (13)
#define IOPRIO_PRIO_MASK ((1UL << IOPRIO_CLASS_SHIFT) - 1)

#define IOPRIO_PRIO_CLASS(mask)    ((mask) >> IOPRIO_CLASS_SHIFT)
#define IOPRIO_PRIO_DATA(mask)     ((mask) & IOPRIO_PRIO_MASK)
#define IOPRIO_PRIO_VALUE(class, data)  (((class) << IOPRIO_CLASS_SHIFT) | data)

enum {
	IOPRIO_CLASS_NONE,
	IOPRIO_CLASS_RT,
	IOPRIO_CLASS_BE,
	IOPRIO_CLASS_IDLE
};

enum {
	IOPRIO_WHO_PROCESS = 1,
	IOPRIO_WHO_PGRP,
	IOPRIO_WHO_USER
};


/*
 * AVR connected to GPIO3
 */

#define PATH_MAINS "/tmp/.pgctl_mains"
#define PATH_LIGHT "/tmp/.pgctl_light"

static void save_state(int index)
{
	FILE *fh;
	const char *states[] = {
		"", "on", "off", "on", "off",
		"10p", "20p", "40p", "60p", "strobe", "on", "off"
	};

	if (index > 11)
		return;

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
		rename(PATH_LIGHT ".new", PATH_LIGHT);
}

int main(int argc, char **argv)
{
	FILE *fh;
	int cmd;
	signed int i;
	const char *commands[] = {
		"none", "mains_on", "mains_off", "light_on", "light_off",
		"light_10p", "light_20p", "light_40p", "light_60p",
		"light_strobe", "fadeup", "fadedown", "", "", "", "", "save",
		"s1_p1_on", "s1_p1_off", "s1_p2_on", "s1_p2_off", "s1_p3_on",
		"s1_p3_off", "s1_p4_on", "s1_p4_off", "s1_all_on", "s1_all_off",
		"s2_p1_on", "s2_p1_off", "s2_p2_on", "s2_p2_off", "s2_p3_on",
		"s2_p3_off", "s2_p4_on", "s2_p4_off", "s2_p5_on", "s2_p5_off",
		"s2_all_on", "s2_all_off", "", "", "",
		"", "", "", "", "",
		"s3_p1_on", "s3_p1_off", "s3_p2_on", "s3_p2_off", "",
		"s3_all_on", "s3_all_off", "", "", "",
		"s4_p1_on", "s4_p1_off", "s4_p2_on", "s4_p2_off", "s4_p3_on",
		"s4_p3_off", "s4_p4_on", "s4_p4_off", "s4_p5_on", "s4_p5_off",
		"", "", "", "", "", "", "", "", "s4_all_on", "s4_all_off"
	};

	if (argc < 2)
		return 1;

	if (nice(-20) == -1)
		fputs("warning: unable to renice\n", stderr);

	if (syscall(SYS_ioprio_set, IOPRIO_WHO_PROCESS, 0, IOPRIO_PRIO_VALUE(IOPRIO_CLASS_RT, 0) == -1))
		fputs("warning: unable to ionice\n", stderr);

	for (cmd = 0; cmd < 256; cmd++) {
		if (!strcmp(argv[1], commands[cmd])) {
			save_state(cmd);
			for (i = -2; i < cmd; i++) {
				usleep(1000);
				fh = fopen("/sys/class/gpio/gpio3/value", "w");
				fputs("1\n", fh);
				fclose(fh);
				usleep(1000);
				fh = fopen("/sys/class/gpio/gpio3/value", "w");
				fputs("0\n", fh);
				fclose(fh);
			}
			usleep(2500000);
			return 0;
		}
	}

	return 1;
}

/*
 * (C) Copyright 2000
 * Paolo Scaffardi, AIRVENT SAM s.p.a - RIMINI(ITALY), arsenio@tin.it
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <stdarg.h>
#include <malloc.h>
#include <console.h>
#include <syscall.h>

void **syscall_tbl;

#ifdef CFG_CONSOLE_IS_IN_ENV
/*
 * if overwrite_console returns 1, the stdin, stderr and stdout
 * are switched to the serial port, else the settings in the
 * environment are used
 */
#ifdef CFG_CONSOLE_OVERWRITE_ROUTINE
extern int overwrite_console (void);
#else
int overwrite_console (void)
{
	return (0);
}
#endif /* CFG_CONSOLE_OVERWRITE_ROUTINE */

#endif /* CFG_CONSOLE_IS_IN_ENV */

static int console_setfile (int file, device_t * dev)
{
	int error = 0;

	if (dev == NULL)
		return -1;

	switch (file) {
	case stdin:
	case stdout:
	case stderr:
		/* Start new device */
		if (dev->start) {
			error = dev->start ();
			/* If it's not started dont use it */
			if (error < 0)
				break;
		}

		/* Assign the new device (leaving the existing one started) */
		stdio_devices[file] = dev;

		/*
		 * Update monitor functions
		 * (to use the console stuff by other applications)
		 */
		switch (file) {
		case stdin:
			syscall_tbl[SYSCALL_GETC] = dev->getc;
			syscall_tbl[SYSCALL_TSTC] = dev->tstc;
			break;
		case stdout:
			syscall_tbl[SYSCALL_PUTC] = dev->putc;
			syscall_tbl[SYSCALL_PUTS] = dev->puts;
			syscall_tbl[SYSCALL_PRINTF] = printf;
			break;
		}
		break;

	default:		/* Invalid file ID */
		error = -1;
	}
	return error;
}

/** PPCBOOT INITIAL CONSOLE-NOT COMPATIBLE FUNCTIONS *************************/

void serial_printf (const char *fmt, ...)
{
	va_list args;
	uint i;
	char printbuffer[CFG_PBSIZE];

	va_start (args, fmt);

	/* For this to work, printbuffer must be larger than
	 * anything we ever want to print.
	 */
	i = vsprintf (printbuffer, fmt, args);
	va_end (args);

	serial_puts (printbuffer);
}

int fgetc (int file)
{
	if (file < MAX_FILES)
		return stdio_devices[file]->getc ();

	return -1;
}

int ftstc (int file)
{
	if (file < MAX_FILES)
		return stdio_devices[file]->tstc ();

	return -1;
}

void fputc (int file, const char c)
{
	if (file < MAX_FILES)
		stdio_devices[file]->putc (c);
}

void fputs (int file, const char *s)
{
	if (file < MAX_FILES)
		stdio_devices[file]->puts (s);
}

void fprintf (int file, const char *fmt, ...)
{
	va_list args;
	uint i;
	char printbuffer[CFG_PBSIZE];

	va_start (args, fmt);

	/* For this to work, printbuffer must be larger than
	 * anything we ever want to print.
	 */
	i = vsprintf (printbuffer, fmt, args);
	va_end (args);

	/* Send to desired file */
	fputs (file, printbuffer);
}

/** PPCBOOT INITIAL CONSOLE-COMPATIBLE FUNCTION *****************************/

int getc (void)
{
	DECLARE_GLOBAL_DATA_PTR;

	if (gd->flags & GD_FLG_DEVINIT) {
		/* Get from the standard input */
		return fgetc (stdin);
	}

	/* Send directly to the handler */
	return serial_getc ();
}

int tstc (void)
{
	DECLARE_GLOBAL_DATA_PTR;

	if (gd->flags & GD_FLG_DEVINIT) {
		/* Test the standard input */
		return ftstc (stdin);
	}

	/* Send directly to the handler */
	return serial_tstc ();
}

void putc (const char c)
{
	DECLARE_GLOBAL_DATA_PTR;

	if (gd->flags & GD_FLG_DEVINIT) {
		/* Send to the standard output */
		fputc (stdout, c);
	} else {
		/* Send directly to the handler */
		serial_putc (c);
	}
}

void puts (const char *s)
{
	DECLARE_GLOBAL_DATA_PTR;

	if (gd->flags & GD_FLG_DEVINIT) {
		/* Send to the standard output */
		fputs (stdout, s);
	} else {
		/* Send directly to the handler */
		serial_puts (s);
	}
}

void printf (const char *fmt, ...)
{
	va_list args;
	uint i;
	char printbuffer[CFG_PBSIZE];

	va_start (args, fmt);

	/* For this to work, printbuffer must be larger than
	 * anything we ever want to print.
	 */
	i = vsprintf (printbuffer, fmt, args);
	va_end (args);

	/* Print the string */
	puts (printbuffer);
}

/* test if ctrl-c was pressed */
static int ctrlc_disabled = 0;	/* see disable_ctrl() */
static int ctrlc_was_pressed = 0;
int ctrlc (void)
{
	DECLARE_GLOBAL_DATA_PTR;

	if (!ctrlc_disabled && gd->have_console) {
		if (tstc ()) {
			switch (getc ()) {
			case 0x03:		/* ^C - Control C */
				ctrlc_was_pressed = 1;
				return 1;
			default:
				break;
			}
		}
	}
	return 0;
}

/* pass 1 to disable ctrlc() checking, 0 to enable.
 * returns previous state
 */
int disable_ctrlc (int disable)
{
	int prev = ctrlc_disabled;	/* save previous state */

	ctrlc_disabled = disable;
	return prev;
}

int had_ctrlc (void)
{
	return ctrlc_was_pressed;
}

void clear_ctrlc (void)
{
	ctrlc_was_pressed = 0;
}

#ifdef CONFIG_MODEM_SUPPORT_DEBUG
char	screen[1024];
char *cursor = screen;
int once = 0;
inline void dbg(const char *fmt, ...)
{
	va_list	args;
	uint	i;
	char	printbuffer[CFG_PBSIZE];

	if (!once) {
		memset(screen, 0, sizeof(screen));
		once++;
	}

	va_start(args, fmt);

	/* For this to work, printbuffer must be larger than
	 * anything we ever want to print.
	 */
	i = vsprintf(printbuffer, fmt, args);
	va_end(args);

	if ((screen + sizeof(screen) - 1 - cursor) < strlen(printbuffer)+1) {
		memset(screen, 0, sizeof(screen));
		cursor = screen;
	}
	sprintf(cursor, printbuffer);
	cursor += strlen(printbuffer);

}
#else
inline void dbg(const char *fmt, ...)
{
}
#endif

/** PPCBOOT INIT FUNCTIONS *************************************************/

int console_assign (int file, char *devname)
{
	int flag, i;

	/* Check for valid file */
	switch (file) {
	case stdin:
		flag = DEV_FLAGS_INPUT;
		break;
	case stdout:
	case stderr:
		flag = DEV_FLAGS_OUTPUT;
		break;
	default:
		return -1;
	}

	/* Check for valid device name */

	for (i = 1; i <= ListNumItems (devlist); i++) {
		device_t *dev = ListGetPtrToItem (devlist, i);

		if (strcmp (devname, dev->name) == 0) {
			if (dev->flags & flag)
				return console_setfile (file, dev);

			return -1;
		}
	}

	return -1;
}

/* Called before relocation - use serial functions */
int console_init_f (void)
{
	DECLARE_GLOBAL_DATA_PTR;

	gd->have_console = 1;
	return (0);
}

#ifdef CFG_CONSOLE_IS_IN_ENV
/* search a device */
device_t *search_device (int flags, char *name)
{
	int i, items;
	device_t *dev = NULL;

	items = ListNumItems (devlist);
	if (name == NULL)
		return dev;

	for (i = 1; i <= items; i++) {
		dev = ListGetPtrToItem (devlist, i);
		if ((dev->flags & flags) && (strcmp (name, dev->name) == 0)) {
			break;
		}
	}
	return dev;
}
#endif /* CFG_CONSOLE_IS_IN_ENV */

#ifdef CFG_CONSOLE_IS_IN_ENV
/* Called after the relocation - use desired console functions */
int console_init_r (void)
{
	char *stdinname, *stdoutname, *stderrname;
	device_t *inputdev = NULL, *outputdev = NULL, *errdev = NULL;

	/* set default handlers at first */
	syscall_tbl[SYSCALL_GETC] = serial_getc;
	syscall_tbl[SYSCALL_TSTC] = serial_tstc;
	syscall_tbl[SYSCALL_PUTC] = serial_putc;
	syscall_tbl[SYSCALL_PUTS] = serial_puts;
	syscall_tbl[SYSCALL_PRINTF] = serial_printf;

	/* stdin stdout and stderr are in environment */
	/* scan for it */
	stdinname  = getenv ("stdin");
	stdoutname = getenv ("stdout");
	stderrname = getenv ("stderr");

	if (overwrite_console () == 0) { /* if not overwritten by config switch */
		inputdev  = search_device (DEV_FLAGS_INPUT,  stdinname);
		outputdev = search_device (DEV_FLAGS_OUTPUT, stdoutname);
		errdev    = search_device (DEV_FLAGS_OUTPUT, stderrname);
	}
	/* if the devices are overwritten or not found, use default device */
	if (inputdev == NULL) {
		inputdev  = search_device (DEV_FLAGS_INPUT,  "serial");
	}
	if (outputdev == NULL) {
		outputdev = search_device (DEV_FLAGS_OUTPUT, "serial");
	}
	if (errdev == NULL) {
		errdev    = search_device (DEV_FLAGS_OUTPUT, "serial");
	}
	/* Initializes output console first */
	if (outputdev != NULL) {
		console_setfile (stdout, outputdev);
	}
	if (errdev != NULL) {
		console_setfile (stderr, errdev);
	}
	if (inputdev != NULL) {
		console_setfile (stdin, inputdev);
	}

#ifndef CFG_CONSOLE_INFO_QUIET
	/* Print information */
	printf ("In:    ");
	if (stdio_devices[stdin] == NULL) {
		printf ("No input devices available!\n");
	} else {
		printf ("%s\n", stdio_devices[stdin]->name);
	}

	printf ("Out:   ");
	if (stdio_devices[stdout] == NULL) {
		printf ("No output devices available!\n");
	} else {
		printf ("%s\n", stdio_devices[stdout]->name);
	}

	printf ("Err:   ");
	if (stdio_devices[stderr] == NULL) {
		printf ("No error devices available!\n");
	} else {
		printf ("%s\n", stdio_devices[stderr]->name);
	}
#endif /* CFG_CONSOLE_INFO_QUIET */

#ifdef CFG_CONSOLE_ENV_OVERWRITE
	/* set the environment variables (will overwrite previous env settings) */
	for (i = 0; i < 3; i++) {
		setenv (stdio_names[i], stdio_devices[i]->name);
	}
#endif /*  CFG_CONSOLE_ENV_OVERWRITE */

#if 0
	/* If nothing usable installed, use only the initial console */
	if ((stdio_devices[stdin] == NULL) && (stdio_devices[stdout] == NULL))
		return (0);
#endif
	return (0);
}

#else /* CFG_CONSOLE_IS_IN_ENV */

/* Called after the relocation - use desired console functions */
int console_init_r (void)
{
	device_t *inputdev = NULL, *outputdev = NULL;
	int i, items = ListNumItems (devlist);

	/* Scan devices looking for input and output devices */
	for (i = 1;
	     (i <= items) && ((inputdev == NULL) || (outputdev == NULL));
	     i++
	    ) {
		device_t *dev = ListGetPtrToItem (devlist, i);

		if ((dev->flags & DEV_FLAGS_INPUT) && (inputdev == NULL)) {
			inputdev = dev;
		}
		if ((dev->flags & DEV_FLAGS_OUTPUT) && (outputdev == NULL)) {
			outputdev = dev;
		}
	}

	/* Initializes output console first */
	if (outputdev != NULL) {
		console_setfile (stdout, outputdev);
		console_setfile (stderr, outputdev);
	}

	/* Initializes input console */
	if (inputdev != NULL) {
		console_setfile (stdin, inputdev);
	}

#ifndef CFG_CONSOLE_INFO_QUIET
	/* Print informations */
	printf ("In:    ");
	if (stdio_devices[stdin] == NULL) {
		printf ("No input devices available!\n");
	} else {
		printf ("%s\n", stdio_devices[stdin]->name);
	}

	printf ("Out:   ");
	if (stdio_devices[stdout] == NULL) {
		printf ("No output devices available!\n");
	} else {
		printf ("%s\n", stdio_devices[stdout]->name);
	}

	printf ("Err:   ");
	if (stdio_devices[stderr] == NULL) {
		printf ("No error devices available!\n");
	} else {
		printf ("%s\n", stdio_devices[stderr]->name);
	}
#endif /* CFG_CONSOLE_INFO_QUIET */

	/* Setting environment variables */
	for (i = 0; i < 3; i++) {
		setenv (stdio_names[i], stdio_devices[i]->name);
	}

#if 0
	/* If nothing usable installed, use only the initial console */
	if ((stdio_devices[stdin] == NULL) && (stdio_devices[stdout] == NULL))
		return (0);
#endif

	return (0);
}

#endif /* CFG_CONSOLE_IS_IN_ENV */

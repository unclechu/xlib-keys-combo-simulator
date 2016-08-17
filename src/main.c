/**
 * Author: Viacheslav Lotsmanov
 * License: GNU/GPLv3 https://raw.githubusercontent.com/unclechu/xlib-keys-combo-simulator/master/LICENSE
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>
#include <X11/keysym.h>

#define streq(a, b) (strcmp(a, b) == 0)

typedef struct {
	const char *name;
	const KeySym sym;
} KeyAlias;

const KeyAlias aliases[] = {
	{ "LShift", XK_ISO_Prev_Group },
	{ "RShift", XK_ISO_Next_Group },
	{ "NumLock", XK_Num_Lock },
	{ "CapsLock", XK_Caps_Lock },
	{ "Level3", XK_ISO_Level3_Shift },
};
const unsigned int aliases_count = sizeof aliases / sizeof *aliases;

unsigned int main(const unsigned int argc, const char **argv)
{
	Display *dpy = XOpenDisplay(NULL);
	
	if (argc <= 1) {
		fprintf(stderr, "No keys to trigger\n");
		return EXIT_FAILURE;
	}
	
	unsigned int keys_count = argc - 1;
	unsigned int *keys = malloc(sizeof *keys * keys_count);
	
	for (unsigned int arg_i = 1; arg_i < argc; ++arg_i) {
		
		const char *keyname = argv[arg_i];
		
		unsigned int is_alias_found = 0;
		for (unsigned int alias_i; alias_i < aliases_count; ++alias_i) {
			const KeyAlias alias = aliases[alias_i];
			if (streq(keyname, alias.name)) {
				keys[arg_i - 1] = XKeysymToKeycode(dpy, alias.sym);
				is_alias_found = 1;
				break;
			}
		}
		
		if (is_alias_found == 0) {
			fprintf(stderr, "Unknown key: %s\n", keyname);
			return EXIT_FAILURE;
		}
	}
	
	// press
	for (unsigned int key_i = 0; key_i < keys_count; ++key_i) {
		XTestFakeKeyEvent(dpy, keys[key_i], True, CurrentTime);
		XFlush(dpy);
	}
	// release
	for (unsigned int key_i = 0; key_i < keys_count; ++key_i) {
		XTestFakeKeyEvent(dpy, keys[key_i], False, CurrentTime);
		XFlush(dpy);
	}
	
	XCloseDisplay(dpy);
	return EXIT_SUCCESS;
}

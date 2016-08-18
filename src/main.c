/**
 * Author: Viacheslav Lotsmanov
 * License: GNU/GPLv3 https://raw.githubusercontent.com/unclechu/xlib-keys-combo-simulator/master/LICENSE
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>
#include <X11/keysym.h>

#define streq(a, b) (strcmp(a, b) == 0)

typedef struct
{
	const char *name;
	const KeySym sym;
} KeyAlias;

enum Mode
{
	ModePressRelease = 1,
	ModePress,
	ModeRelease,
	ModeToggle // TODO implement
};

typedef unsigned int ModeEnum;

typedef struct
{
	const ModeEnum mode;
	const KeyCode keycode;
} KeyAction;

const KeyAlias aliases[] = {
	{ "LShift", XK_ISO_Prev_Group },
	{ "RShift", XK_ISO_Next_Group },
	{ "NumLock", XK_Num_Lock },
	{ "CapsLock", XK_Caps_Lock },
	{ "Level3", XK_ISO_Level3_Shift },
};
const unsigned int aliases_count = sizeof aliases / sizeof *aliases;

bool is_opt(const char *arg)
{
	if (strlen(arg) < 2) {
		return false;
	}
	char first_chars[3];
	strncpy(first_chars, arg, 2);
	first_chars[2] = '\0';
	return streq(first_chars, "--") ? true : false;
}

unsigned int main(const unsigned int argc, const char **argv)
{
	assert(argc >= 1);
	Display *dpy = XOpenDisplay(NULL);
	
	unsigned int key_actions_count = 0;
	KeyAction **key_actions = malloc(sizeof **key_actions * (argc - 1));
	ModeEnum cur_mode = ModePressRelease;
	
	// parse arguments
	for (unsigned int arg_i = 1; arg_i < argc; ++arg_i) {
		
		const char *arg = argv[arg_i];
		
		if (is_opt(arg) == true) {
			if (streq(arg, "--press-release")) {
				cur_mode = ModePressRelease;
			} else if (streq(arg, "--press")) {
				cur_mode = ModePress;
			} else if (streq(arg, "--release")) {
				cur_mode = ModeRelease;
			} else {
				fprintf(stderr, "Unknown option: %s\n", arg);
				return EXIT_FAILURE;
			}
			continue;
		}
		
		bool is_alias_found = false;
		for (unsigned int alias_i = 0; alias_i < aliases_count; ++alias_i) {
			const KeyAlias alias = aliases[alias_i];
			if (streq(arg, alias.name)) {
				
				KeyAction action = {
					cur_mode,
					XKeysymToKeycode(dpy, alias.sym)
				};
				KeyAction *action_pointer = malloc(sizeof *action_pointer);
				memcpy(action_pointer, &action, sizeof action);
				
				key_actions[key_actions_count++] = action_pointer;
				is_alias_found = true;
				break;
			}
		}
		
		if (is_alias_found == false) {
			fprintf(stderr, "Unknown key: %s\n", arg);
			return EXIT_FAILURE;
		}
	}
	
	// press
	for (unsigned int key_i = 0; key_i < key_actions_count; ++key_i) {
		const KeyAction *action = key_actions[key_i];
		if (action->mode == ModePressRelease || action->mode == ModePress) {
			XTestFakeKeyEvent(dpy, action->keycode, True, CurrentTime);
			XFlush(dpy);
		}
	}
	// release
	for (unsigned int key_i = 0; key_i < key_actions_count; ++key_i) {
		const KeyAction *action = key_actions[key_i];
		if (action->mode == ModePressRelease || action->mode == ModeRelease) {
			XTestFakeKeyEvent(dpy, action->keycode, False, CurrentTime);
			XFlush(dpy);
		}
	}
	
	XCloseDisplay(dpy);
	return EXIT_SUCCESS;
}

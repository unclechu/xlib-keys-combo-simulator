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
#include <X11/XKBlib.h>

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
	ModeToggle
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
	{ "Level3Shift", XK_ISO_Level3_Shift },
};
const unsigned int aliases_count = sizeof aliases / sizeof *aliases;

const bool is_opt(const char *arg)
{
	if (strlen(arg) < 2) {
		return false;
	}
	char first_chars[3];
	strncpy(first_chars, arg, 2);
	first_chars[2] = '\0';
	return streq(first_chars, "--") ? true : false;
}

const KeySym get_keysym_by_name(const char *name)
{
	for (unsigned int alias_i = 0; alias_i < aliases_count; ++alias_i) {
		const KeyAlias *alias = &aliases[alias_i];
		if (streq(alias->name, name)) {
			return alias->sym;
		}
	}
	return -1; // alias not found
}

bool is_pressed_keys_fetched = false;
char keys_return[32];
const unsigned int keys_return_count = sizeof keys_return / sizeof *keys_return;

void fetch_pressed_keys(Display *dpy)
{
	if (is_pressed_keys_fetched == false) {
		XQueryKeymap(dpy, keys_return);
		is_pressed_keys_fetched = true;
	}
}

const bool is_key_pressed(Display *dpy, const char *name)
{
	const KeySym keysym = get_keysym_by_name(name);
	assert(keysym != -1);
	fetch_pressed_keys(dpy);
	
	for (unsigned int i = 0; i < keys_return_count; ++i) {
		
		if (keys_return[i] == '\0') {
			continue;
		}
		
		for (
			int pos = 0, num = keys_return[i];
			pos < 8;
			++pos, num /= 2
		) {
			if ((num & 0x01) != 1) {
				continue;
			}
			
			// see also http://stackoverflow.com/questions/9838385/replace-of-xkeycodetokeysym#22418839
			const unsigned int mask = (
				streq(name, "LShift") || streq(name, "RShift") ? ShiftMask : 0
			);
			const KeySym pressedKeysym =
				XkbKeycodeToKeysym(dpy, (i*8 + pos), 0, mask);
			
			if (pressedKeysym == keysym) {
				return true;
			}
		}
	}
	
	return false;
}

const unsigned int main(const unsigned int argc, const char **argv)
{
	assert(argc >= 1);
	Display *dpy = XOpenDisplay(NULL);
	
	unsigned int key_actions_count = 0;
	KeyAction key_actions[argc - 1];
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
			} else if (streq(arg, "--toggle")) {
				cur_mode = ModeToggle;
			} else {
				fprintf(stderr, "Unknown option: %s\n", arg);
				return EXIT_FAILURE;
			}
			continue;
		}
		
		const KeySym keysym = get_keysym_by_name(arg);
		if (keysym == -1) {
			fprintf(stderr, "Unknown key name '%s'\n", arg);
			return EXIT_FAILURE;
		}
		
		ModeEnum action_mode = cur_mode;
		if (cur_mode == ModeToggle) {
			action_mode = is_key_pressed(dpy, arg) ? ModeRelease : ModePress;
		}
		KeyAction action = {
			action_mode,
			XKeysymToKeycode(dpy, keysym)
		};
		memcpy(&key_actions[key_actions_count++], &action, sizeof action);
	}
	
	// press
	for (unsigned int key_i = 0; key_i < key_actions_count; ++key_i) {
		const KeyAction *action = &key_actions[key_i];
		if (action->mode == ModePressRelease || action->mode == ModePress) {
			XTestFakeKeyEvent(dpy, action->keycode, True, CurrentTime);
			XFlush(dpy);
		}
	}
	// release
	for (unsigned int key_i = 0; key_i < key_actions_count; ++key_i) {
		const KeyAction *action = &key_actions[key_i];
		if (action->mode == ModePressRelease || action->mode == ModeRelease) {
			XTestFakeKeyEvent(dpy, action->keycode, False, CurrentTime);
			XFlush(dpy);
		}
	}
	
	XCloseDisplay(dpy);
	return EXIT_SUCCESS;
}

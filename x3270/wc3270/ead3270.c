/*
 * Copyright (c) 2008-2009, 2013, 2015 Paul Mattes.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Paul Mattes nor his contributors may be used
 *       to endorse or promote products derived from this software without
 *       specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY PAUL MATTES "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL PAUL MATTES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *	ead3270.c
 *		A Windows console 3270 Terminal Emulator
 *		Application Data directory explorer.
 */

#if !defined(_WIN32) /*[*/
#error For Windows only.
#endif /*]*/

#include <stdio.h>
#include "wincmn.h"

#include <limits.h>

#include "windirs.h"

#define COMMAND "start explorer.exe"

static int
explore(char *dir)
{
	size_t sl;
	char cmd[sizeof(COMMAND) + 2 + MAX_PATH + 1];

	sl = strlen(dir);
	if (sl > 1 && dir[sl - 1] == '\\') {
		dir[sl - 1] = '\0';
	}

	/* Run it. */
	sprintf(cmd, "%s \"%s\"", COMMAND, dir);
	system(cmd);
	return 0;
}

int
main(int argc, char *argv[])
{
	char *appdata = NULL;
	char *commonappdata = NULL;

	/* Get the application data directories. */
	if (get_dirs(NULL, "wc3270", NULL, NULL, &appdata, NULL,
		    &commonappdata, NULL) < 0) {
		fprintf(stderr, "get_dirs failed\n");
		return 1;
	}

	/* Explore them. */
	(void) explore(appdata);
	if (commonappdata) {
		(void) explore(commonappdata);
	}

	return 0;
}

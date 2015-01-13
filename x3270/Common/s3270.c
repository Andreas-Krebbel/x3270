/*
 * Copyright (c) 1993-2009, 2013-2015 Paul Mattes.
 * Copyright (c) 1990, Jeff Sparkes.
 * Copyright (c) 1989, Georgia Tech Research Corporation (GTRC), Atlanta,
 *  GA 30332.
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
 *     * Neither the names of Paul Mattes, Jeff Sparkes, GTRC nor their
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY PAUL MATTES, JEFF SPARKES AND GTRC "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL PAUL MATTES, JEFF SPARKES OR
 * GTRC BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *	s3270.c
 *		A displayless 3270 Terminal Emulator
 *		Main proceudre.
 */

#include "globals.h"
#if !defined(_WIN32) /*[*/
#include <sys/wait.h>
#include <signal.h>
#endif /*]*/
#include <errno.h>
#include "appres.h"
#include "3270ds.h"
#include "resources.h"

#include "actionsc.h"
#include "bind-optc.h"
#include "charsetc.h"
#include "ctlrc.h"
#include "unicodec.h"
#include "ftc.h"
#include "gluec.h"
#include "hostc.h"
#include "httpd-corec.h"
#include "httpd-nodesc.h"
#include "httpd-ioc.h"
#include "idlec.h"
#include "keymapc.h"
#include "kybdc.h"
#include "macrosc.h"
#include "menubarc.h"
#include "nvtc.h"
#include "popupsc.h"
#include "print_screen.h"
#include "screenc.h"
#include "selectc.h"
#include "tablesc.h"
#include "telnetc.h"
#include "togglesc.h"
#include "trace.h"
#include "utilc.h"
#include "xioc.h"

#if defined(_WIN32) /*[*/
#include "w3miscc.h"
#include "windirsc.h"
#include "winversc.h"
#endif /*]*/

#if defined(_WIN32) /*[*/
char *instdir = NULL;
char *myappdata = NULL;
char *commonappdata = NULL;
char *mydesktop = NULL;
#endif /*]*/

static void s3270_register(void);

void
usage(const char *msg)
{
	if (msg != NULL)
	    	fprintf(stderr, "%s\n", msg);
	fprintf(stderr, "Usage: %s [options] [ps:][LUname@]hostname[:port]\n",
		programname);
	fprintf(stderr, "Options:\n");
	cmdline_help(False);
	exit(1);
}

static void
main_connect(Boolean ignored)
{       
	if (CONNECTED || appres.disconnect_clear)
                ctlr_erase(True);
} 

int
main(int argc, char *argv[])
{
	const char	*cl_hostname = NULL;

#if defined(_WIN32) /*[*/
	(void) get_version_info();
	if (get_dirs(argv[0], "wc3270", &instdir, NULL, &myappdata,
		    NULL, &commonappdata, NULL) < 0)
		exit(1);
	if (sockstart() < 0)
	    	exit(1);
#endif /*]*/

	/*
	 * Call the module registration functions, to build up the tables of
	 * actions, options and callbacks.
	 */
	ctlr_register();
	ft_register();
	host_register();
	idle_register();
	kybd_register();
	macros_register();
	nvt_register();
	print_screen_register();
	s3270_register();
	toggles_register();
	trace_register();
	xio_register();

	argc = parse_command_line(argc, (const char **)argv, &cl_hostname);

	if (charset_init(appres.charset) != CS_OKAY) {
		xs_warning("Cannot find charset \"%s\"", appres.charset);
		(void) charset_init(NULL);
	}
	model_init();
	ctlr_init(-1);
	ctlr_reinit(-1);
	idle_init();
	if (appres.httpd_port) {
	    struct sockaddr *sa;
	    socklen_t sa_len;

	    if (!parse_bind_opt(appres.httpd_port, &sa, &sa_len)) {
		xs_warning("Invalid -httpd port \"%s\"", appres.httpd_port);
	    } else {
		httpd_objects_init();
		hio_init(sa, sa_len);
	    }
	}
	ft_init();
	hostfile_init();

#if !defined(_WIN32) /*[*/
	/* Make sure we don't fall over any SIGPIPEs. */
	(void) signal(SIGPIPE, SIG_IGN);
#endif /*]*/

	/* Handle initial toggle settings. */
	initialize_toggles();

#if defined(HAVE_LIBSSL) /*[*/
	ssl_base_init(NULL, NULL);
#endif /*]*/

	/* Connect to the host. */
	if (cl_hostname != NULL) {
		if (host_connect(cl_hostname) < 0)
			exit(1);
		/* Wait for negotiations to complete or fail. */
		while (!IN_NVT && !IN_3270) {
			(void) process_events(True);
			if (!PCONNECTED)
				exit(1);
		}
	}

	/* Prepare to run a peer script. */
	peer_script_init();

	/* Process events forever. */
	while (1) {
		(void) process_events(True);

#if !defined(_WIN32) /*[*/
		if (children && waitpid(-1, (int *)0, WNOHANG) > 0)
			--children;
#endif /*]*/
	}
}

/**
 * Main module registration.
 */
static void
s3270_register(void)
{
    register_schange(ST_CONNECT, main_connect);
    register_schange(ST_3270_MODE, main_connect);
}
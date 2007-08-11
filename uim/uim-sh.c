/*
  uim-sh.c: uim interactive shell for debugging, batch processing and
            serving as generic inferior process

  Copyright (c) 2003-2007 uim Project http://code.google.com/p/uim/

  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

  1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
  3. Neither the name of authors nor the names of its contributors
     may be used to endorse or promote products derived from this software
     without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS'' AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
  OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
  SUCH DAMAGE.

*/

#include <stdlib.h>

#include "uim.h"
#include "uim-scm.h"
#include "uim-scm-abbrev.h"

#ifdef LIBEDIT
#include "editline.h"
#endif

#ifdef UIM_SH_USE_EXIT_HOOK
extern int uim_siod_fatal;
#endif

struct uim_sh_args {
  int argc;
  char **argv;
};
static void *uim_sh(struct uim_sh_args *c_args);

int
main(int argc, char *argv[])
{
  struct uim_sh_args c_args;
  int err;

  /* TODO: be able to suppress ordinary initialization process */
  uim_init();

  c_args.argc = argc;
  c_args.argv = argv;
  err = (int)uim_scm_call_with_gc_ready_stack((uim_gc_gate_func_ptr)uim_sh,
					      &c_args);
  if (err)
    return err;

  uim_quit();

  return EXIT_SUCCESS;
}

static void *
uim_sh(struct uim_sh_args *c_args)
{
  uim_lisp args;

#ifdef LIBEDIT
  editline_init();
#endif

  uim_scm_require_file("uim-sh.scm");
#ifdef UIM_SH_USE_EXIT_HOOK
  /*
     is not working even if uim_siod_fatal is accessible. outermost
     *catch affects me?
  */
  if (uim_siod_fatal)
    return (void *)EXIT_FAILURE;
#endif

  args = uim_scm_array2list((void **)c_args->argv, c_args->argc,
			    (uim_lisp (*)(void *))uim_scm_make_str);
  uim_scm_callf("uim-sh", "o", args);

#ifdef UIM_SH_USE_EXIT_HOOK
  /* is not working even if uim_siod_fatal is accessible. outermost
   * *catch affects me?
   */
  if (uim_siod_fatal)
    return (void *)EXIT_FAILURE;
#endif

#ifdef LIBEDIT
  editline_quit();
#endif

  return (void *)EXIT_SUCCESS;
}

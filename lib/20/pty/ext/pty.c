#include	"ruby/config.h"
#ifdef RUBY_EXTCONF_H
#include RUBY_EXTCONF_H
#endif
#include	<stdlib.h>
#include	<stdio.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<sys/file.h>
#include	<fcntl.h>
#include	<errno.h>
#include	<pwd.h>
#ifdef HAVE_SYS_IOCTL_H
#include	<sys/ioctl.h>
#endif
#ifdef HAVE_LIBUTIL_H
#include	<libutil.h>
#endif
#ifdef HAVE_UTIL_H
#include	<util.h>
#endif
#ifdef HAVE_PTY_H
#include	<pty.h>
#endif
#ifdef HAVE_SYS_WAIT_H
#include <sys/wait.h>
#else
#ifndef WIFSTOPPED
#define WIFSTOPPED(status)    (((status) & 0xff) == 0x7f)
#endif
#endif
#include <ctype.h>

#include "ruby/ruby.h"
#include "ruby/io.h"
#include "ruby/util.h"

#include <signal.h>
#ifdef HAVE_SYS_STROPTS_H
#include <sys/stropts.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#if defined(HAVE_OPENPTY) && !defined(HAVE_PTY_H)
/* Normally, the following header would be used:
 * #include <util.h>
 *
 * but MRI has an include file "util.h" which may be picked up depending on the
 * order of include directories. Rather than futz with that, we just add the
 * prototype.
 */
#include <sys/termios.h>

int openpty(int *amaster, int *aslave, char *name, struct termios *termp,
         struct winsize *winp);
#endif

#ifdef TIOCSCTTY
#include <sys/ioctl.h>
#endif

#define	DEVICELEN	16

#if !defined(HAVE_OPENPTY)
#if defined(__hpux)
static const
char  MasterDevice[] = "/dev/ptym/pty%s",
      SlaveDevice[] =  "/dev/pty/tty%s",
      *const deviceNo[] = {
		"p0","p1","p2","p3","p4","p5","p6","p7",
		"p8","p9","pa","pb","pc","pd","pe","pf",
		"q0","q1","q2","q3","q4","q5","q6","q7",
		"q8","q9","qa","qb","qc","qd","qe","qf",
		"r0","r1","r2","r3","r4","r5","r6","r7",
		"r8","r9","ra","rb","rc","rd","re","rf",
		"s0","s1","s2","s3","s4","s5","s6","s7",
		"s8","s9","sa","sb","sc","sd","se","sf",
		"t0","t1","t2","t3","t4","t5","t6","t7",
		"t8","t9","ta","tb","tc","td","te","tf",
		"u0","u1","u2","u3","u4","u5","u6","u7",
		"u8","u9","ua","ub","uc","ud","ue","uf",
		"v0","v1","v2","v3","v4","v5","v6","v7",
		"v8","v9","va","vb","vc","vd","ve","vf",
		"w0","w1","w2","w3","w4","w5","w6","w7",
		"w8","w9","wa","wb","wc","wd","we","wf",
		0,
	};
#elif defined(_IBMESA)  /* AIX/ESA */
static const
char  MasterDevice[] = "/dev/ptyp%s",
      SlaveDevice[] = "/dev/ttyp%s",
      *const deviceNo[] = {
"00","01","02","03","04","05","06","07","08","09","0a","0b","0c","0d","0e","0f",
"10","11","12","13","14","15","16","17","18","19","1a","1b","1c","1d","1e","1f",
"20","21","22","23","24","25","26","27","28","29","2a","2b","2c","2d","2e","2f",
"30","31","32","33","34","35","36","37","38","39","3a","3b","3c","3d","3e","3f",
"40","41","42","43","44","45","46","47","48","49","4a","4b","4c","4d","4e","4f",
"50","51","52","53","54","55","56","57","58","59","5a","5b","5c","5d","5e","5f",
"60","61","62","63","64","65","66","67","68","69","6a","6b","6c","6d","6e","6f",
"70","71","72","73","74","75","76","77","78","79","7a","7b","7c","7d","7e","7f",
"80","81","82","83","84","85","86","87","88","89","8a","8b","8c","8d","8e","8f",
"90","91","92","93","94","95","96","97","98","99","9a","9b","9c","9d","9e","9f",
"a0","a1","a2","a3","a4","a5","a6","a7","a8","a9","aa","ab","ac","ad","ae","af",
"b0","b1","b2","b3","b4","b5","b6","b7","b8","b9","ba","bb","bc","bd","be","bf",
"c0","c1","c2","c3","c4","c5","c6","c7","c8","c9","ca","cb","cc","cd","ce","cf",
"d0","d1","d2","d3","d4","d5","d6","d7","d8","d9","da","db","dc","dd","de","df",
"e0","e1","e2","e3","e4","e5","e6","e7","e8","e9","ea","eb","ec","ed","ee","ef",
"f0","f1","f2","f3","f4","f5","f6","f7","f8","f9","fa","fb","fc","fd","fe","ff",
		};
#elif !defined(HAVE_PTSNAME)
static const
char  MasterDevice[] = "/dev/pty%s",
      SlaveDevice[] = "/dev/tty%s",
      *const deviceNo[] = {
		"p0","p1","p2","p3","p4","p5","p6","p7",
		"p8","p9","pa","pb","pc","pd","pe","pf",
		"q0","q1","q2","q3","q4","q5","q6","q7",
		"q8","q9","qa","qb","qc","qd","qe","qf",
		"r0","r1","r2","r3","r4","r5","r6","r7",
		"r8","r9","ra","rb","rc","rd","re","rf",
		"s0","s1","s2","s3","s4","s5","s6","s7",
		"s8","s9","sa","sb","sc","sd","se","sf",
		0,
	};
#endif
#endif /* !defined(HAVE_OPENPTY) */

#ifndef HAVE_SETEUID
# ifdef HAVE_SETREUID
#  define seteuid(e)	setreuid(-1, (e))
# else /* NOT HAVE_SETREUID */
#  ifdef HAVE_SETRESUID
#   define seteuid(e)	setresuid(-1, (e), -1)
#  else /* NOT HAVE_SETRESUID */
    /* I can't set euid. (;_;) */
#  endif /* HAVE_SETRESUID */
# endif /* HAVE_SETREUID */
#endif /* NO_SETEUID */

static VALUE eChildExited;

/* Returns the exit status of the child for which PTY#check
 * raised this exception
 */
static VALUE
echild_status(VALUE self)
{
  return rb_ivar_get(self, rb_intern("status"));
}

struct pty_info {
  int fd;
  int child_pid;
  VALUE thread;
};

static void getDevice _((int*, int*, char [DEVICELEN]));

struct exec_info {
  int argc;
  VALUE *argv;
};

static VALUE pty_exec _((VALUE v));

static VALUE
pty_exec(v)
    VALUE v;
{
  struct exec_info *arg = (struct exec_info *)v;
  return rb_funcall2(rb_mKernel, rb_intern("exec"),
                     arg->argc, arg->argv);
}

static void
establishShell(int argc, VALUE *argv, struct pty_info *info,
	       char SlaveName[DEVICELEN])
{
  int i, master, slave;
  char *p, tmp, *getenv();
  struct passwd *pwent;
  VALUE v;
  struct exec_info arg;
  int status;

  if (argc == 0) {
    char *shellname;

    if ((p = getenv("SHELL")) != NULL) {
      shellname = p;
    }
    else {
      pwent = getpwuid(getuid());
      if (pwent && pwent->pw_shell)
        shellname = pwent->pw_shell;
      else
        shellname = "/bin/sh";
    }
    v = rb_str_new2(shellname);
    argc = 1;
    argv = &v;
  }
  getDevice(&master, &slave, SlaveName);

  info->thread = rb_thread_current();
  VALUE child_pid = rb_funcall(rb_mKernel, rb_intern("fork"), 0);

  /* if(i < 0) { */
    /* close(master); */
    /* close(slave); */
    /* rb_sys_fail("fork failed"); */
  /* } */

  if(child_pid == Qnil) { /* child */
    /*
     * Set free from process group and controlling terminal
     */
#ifdef HAVE_SETSID
    (void) setsid();
#else /* HAS_SETSID */
# ifdef HAVE_SETPGRP
#  ifdef SETGRP_VOID
    if (setpgrp() == -1)
      perror("setpgrp()");
#  else /* SETGRP_VOID */
    if (setpgrp(0, getpid()) == -1)
      rb_sys_fail("setpgrp()");
    if ((i = open("/dev/tty", O_RDONLY)) < 0)
      rb_sys_fail("/dev/tty");
    else {
      if (ioctl(i, TIOCNOTTY, (char *)0))
        perror("ioctl(TIOCNOTTY)");
      close(i);
    }
#  endif /* SETGRP_VOID */
# endif /* HAVE_SETPGRP */
#endif /* HAS_SETSID */

    /*
     * obtain new controlling terminal
     */
#if defined(TIOCSCTTY)
    close(master);
    (void) ioctl(slave, TIOCSCTTY, (char *)0);
    /* errors ignored for sun */
#else
    close(slave);
    slave = open(SlaveName, O_RDWR);
    if (slave < 0) {
      perror("open: pty slave");
      _exit(1);
    }
    close(master);
#endif
    write(slave, "", 1);
    dup2(slave,0);
    dup2(slave,1);
    dup2(slave,2);
    close(slave);
#if defined(HAVE_SETEUID) || defined(HAVE_SETREUID) || defined(HAVE_SETRESUID)
    seteuid(getuid());
#endif

    arg.argc = argc;
    arg.argv = argv;
    rb_protect(pty_exec, (VALUE)&arg, &status);
    sleep(1);
    _exit(1);
  }

  read(master, &tmp, 1);
  close(slave);

  info->child_pid = FIX2INT(child_pid);
  info->fd = master;
}

static int
no_mesg(char *slavedevice, int nomesg)
{
  if (nomesg) {
    return chmod(slavedevice, 0600);
  } else {
    return 0;
  }
}

static int
get_device_once(int *master, int *slave, char SlaveName[DEVICELEN], int nomesg, int fail)
{
#if defined(HAVE_POSIX_OPENPT)
  int masterfd = -1, slavefd = -1;
  char *slavedevice;
  struct sigaction dfl, old;

  dfl.sa_handler = SIG_DFL;
  dfl.sa_flags = 0;
  sigemptyset(&dfl.sa_mask);

  if ((masterfd = posix_openpt(O_RDWR|O_NOCTTY)) == -1) goto error;
  if (sigaction(SIGCHLD, &dfl, &old) == -1) goto error;
  if (grantpt(masterfd) == -1) goto grantpt_error;
  if (sigaction(SIGCHLD, &old, NULL) == -1) goto error;
  if (unlockpt(masterfd) == -1) goto error;
  if ((slavedevice = ptsname(masterfd)) == NULL) goto error;
  if (no_mesg(slavedevice, nomesg) == -1) goto error;
  if ((slavefd = open(slavedevice, O_RDWR|O_NOCTTY, 0)) == -1) goto error;

#if defined I_PUSH && !defined linux
  if (ioctl(slavefd, I_PUSH, "ptem") == -1) goto error;
  if (ioctl(slavefd, I_PUSH, "ldterm") == -1) goto error;
  if (ioctl(slavefd, I_PUSH, "ttcompat") == -1) goto error;
#endif

  *master = masterfd;
  *slave = slavefd;
  strlcpy(SlaveName, slavedevice, DEVICELEN);
  return 0;

grantpt_error:
  sigaction(SIGCHLD, &old, NULL);
error:
  if (slavefd != -1) close(slavefd);
  if (masterfd != -1) close(masterfd);
  if (fail) {
    rb_raise(rb_eRuntimeError, "can't get Master/Slave device");
  }
  return -1;
#elif defined HAVE_OPENPTY
/*
 * Use openpty(3) of 4.3BSD Reno and later,
 * or the same interface function.
 */
  if (openpty(master, slave, SlaveName,
     (struct termios *)0, (struct winsize *)0) == -1) {
    if (!fail) return -1;
      rb_raise(rb_eRuntimeError, "openpty() failed");
  }
  if (no_mesg(SlaveName, nomesg) == -1) {
    if (!fail) return -1;
    rb_raise(rb_eRuntimeError, "can't chmod slave pty");
  }

  return 0;

#elif defined HAVE__GETPTY
  char *name;
  mode_t mode = nomesg ? 0600 : 0622;

  if (!(name = _getpty(master, O_RDWR, mode, 0))) {
    if (!fail) return -1;
    rb_raise(rb_eRuntimeError, "_getpty() failed");
  }

  *slave = open(name, O_RDWR);
  /* error check? */
  strlcpy(SlaveName, name, DEVICELEN);

  return 0;
#elif defined(HAVE_PTSNAME)
  int	 masterfd = -1, slavefd = -1;
  char *slavedevice;
  void (*s)();

  extern char *ptsname(int);
  extern int unlockpt(int);
  extern int grantpt(int);

  if((masterfd = open("/dev/ptmx", O_RDWR, 0)) == -1) goto error;
  s = signal(SIGCHLD, SIG_DFL);
  if(grantpt(masterfd) == -1) goto error;
  signal(SIGCHLD, s);
  if(unlockpt(masterfd) == -1) goto error;
  if((slavedevice = ptsname(masterfd)) == NULL) goto error;
  if (no_mesg(slavedevice, nomesg) == -1) goto error;
  if((slavefd = open(slavedevice, O_RDWR, 0)) == -1) goto error;
#if defined I_PUSH && !defined linux
  if(ioctl(slavefd, I_PUSH, "ptem") == -1) goto error;
  if(ioctl(slavefd, I_PUSH, "ldterm") == -1) goto error;
  ioctl(slavefd, I_PUSH, "ttcompat");
#endif
  *master = masterfd;
  *slave = slavefd;
  strlcpy(SlaveName, slavedevice, DEVICELEN);
  return 0;

error:
  if (slavefd != -1) close(slavefd);
  if (masterfd != -1) close(masterfd);
  if (fail) rb_raise(rb_eRuntimeError, "can't get Master/Slave device");
  return -1;
#else
  int	 masterfd = -1, slavefd = -1;
  const char *const *p;
  char MasterName[DEVICELEN];

  for (p = deviceNo; *p != NULL; p++) {
    snprintf(MasterName, sizeof MasterName, MasterDevice, *p);
    if ((masterfd = open(MasterName,O_RDWR,0)) >= 0) {
      *master = masterfd;
      snprintf(SlaveName, DEVICELEN, SlaveDevice, *p);
      if ((slavefd = open(SlaveName,O_RDWR,0)) >= 0) {
        *slave = slavefd;
        if (chown(SlaveName, getuid(), getgid()) != 0) goto error;
        if (chmod(SlaveName, nomesg ? 0600 : 0622) != 0) goto error;
        return 0;
      }
      close(masterfd);
    }
  }
error:
  if (slavefd != -1) close(slavefd);
  if (masterfd != -1) close(masterfd);
  if (fail) rb_raise(rb_eRuntimeError, "can't get %s", SlaveName);
  return -1;
#endif
}

static void
getDevice(int *master, int *slave, char SlaveName[DEVICELEN], int nomesg)
{
  if (get_device_once(master, slave, SlaveName, nomesg, 0)) {
    get_device_once(master, slave, SlaveName, nomesg, 1);
  }
}

static VALUE
pty_get_device(VALUE klass)
{
  int master_fd, slave_fd;
  char slavename[DEVICELEN];
  VALUE ary;

  getDevice(&master_fd, &slave_fd, slavename, 1);
  ary = rb_ary_new3(3, INT2NUM(master_fd), INT2NUM(slave_fd),
                    rb_str_new_cstr(slavename));
  return ary;
}

/*
 * call-seq:
 *   PTY.spawn(command_line)  { |r, w, pid| ... }
 *   PTY.spawn(command_line)  => [r, w, pid]
 *   PTY.spawn(command, args, ...)  { |r, w, pid| ... }
 *   PTY.spawn(command, args, ...)  => [r, w, pid]
 *   PTY.getpty(command_line)  { |r, w, pid| ... }
 *   PTY.getpty(command_line)  => [r, w, pid]
 *   PTY.getpty(command, args, ...)  { |r, w, pid| ... }
 *   PTY.getpty(command, args, ...)  => [r, w, pid]
 *
 * Spawns the specified command on a newly allocated pty.
 *
 * The command's controlling tty is set to the slave device of the pty
 * and its standard input/output/error is redirected to the slave device.
 *
 * <tt>command_line</tt>:: The full command line to run
 * <tt>command</tt>:: The command to run, as a String.
 * <tt>args</tt>:: Zero or more arguments, as Strings, representing
 *                 the arguments to +command+
 *
 * In the non-block form this returns an array of size three,
 * <tt>[r, w, pid]</tt>.  In the block form the block will be called with
 * these as arguments, <tt>|r,w,pid|</tt>:
 *
 * +r+:: An IO that can be read from that contains the command's
 *       standard output and standard error
 * +w+:: An IO that can be written to that is the command's
 *       standard input
 * +pid+:: The process identifier for the command.
 */
static VALUE
pty_getpty(int argc, VALUE *argv, VALUE self)
{

  VALUE res, rport, wport;
  struct pty_info info;
  struct pty_info thinfo;
  rb_io_t *wfptr,*rfptr;
  char SlaveName[DEVICELEN];

  establishShell(argc, argv, &info, SlaveName);

  rport = rb_funcall(rb_cFile, rb_intern("new"), 2, INT2FIX(info.fd),
      rb_str_new2("r"));

  rb_ivar_set(rport, rb_intern("@path"), rb_str_new2(SlaveName));

  wport = rb_funcall(rb_cFile, rb_intern("new"), 2, INT2FIX(dup(info.fd)),
      rb_str_new2("r"));

  rb_ivar_set(wport, rb_intern("@path"), rb_str_new2(SlaveName));
  rb_funcall(wport, rb_intern("sync="), 1, Qtrue);

  res = rb_ary_new3(3, (VALUE)rport, (VALUE)wport, INT2FIX(info.child_pid));

  if (rb_block_given_p()) {
    rb_yield(res);
    return Qnil;
  }
  return res;
}

/*
 * Document-class: PTY::ChildExited
 *
 * Thrown when PTY#check is called for a pid that represents a process that
 * has exited.
 */

/*
 * Document-class: PTY
 *
 * Creates and managed pseudo terminals (PTYs).  See also
 * http://en.wikipedia.org/wiki/Pseudo_terminal
 */

void
Init_pty()
{
  VALUE cPTY = rb_define_module("PTY");
  rb_define_module_function(cPTY, "getpty", pty_getpty, -1);
  rb_define_module_function(cPTY, "spawn", pty_getpty, -1);
  rb_define_singleton_method(cPTY, "__get_device__", pty_get_device, 0);

  eChildExited = rb_define_class_under(cPTY, "ChildExited", rb_eRuntimeError);
  rb_define_method(eChildExited, "status", echild_status, 0);
}

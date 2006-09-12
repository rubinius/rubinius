#ifndef __INTERNAL_H__
#define __INTERNAL_H__

#include "st.h"
#include "re.h"
#include "node.h"
#include "env.h"

struct gc_list {
    VALUE *varptr;
    struct gc_list *next;
};

struct global_entry {
    struct global_variable *var;
    ID id;
};


#undef GC_DEBUG
/* #define GC_DEBUG */

typedef struct RVALUE {
    union {
	struct {
	    unsigned long flags;	/* always 0 for freed obj */
	    struct RVALUE *next;
	} free;
	struct RBasic  basic;
	struct RObject object;
	struct RClass  klass;
	struct RFloat  flonum;
	struct RString string;
	struct RArray  array;
	struct RRegexp regexp;
	struct RHash   hash;
	struct RData   data;
	struct RStruct rstruct;
	struct RBignum bignum;
	struct RFile   file;
	struct RNode   node;
	struct RMatch  match;
	struct RVarmap varmap; 
	struct SCOPE   scope;
    } as;
#ifdef GC_DEBUG
    char *file;
    int   line;
#endif
} RVALUE;

#define HEAP_MIN_SLOTS 10000

struct heaps_slot {
    RVALUE *slot;
    int limit;
};

#define MARK_STACK_MAX 1024

/* from eval.c */

struct cache_entry {		/* method hash table. */
    ID mid;			/* method's id */
    ID mid0;			/* method's original id */
    VALUE klass;		/* receiver's class */
    VALUE origin;		/* where method defined  */
    NODE *method;
    int noex;
};

#define CACHE_SIZE 0x800
#define CACHE_MASK 0x7ff
#define EXPR1(c,m) ((((c)>>3)^(m))&CACHE_MASK)

struct BLOCK {
    NODE *var;
    NODE *body;
    VALUE self;
    struct FRAME frame;
    struct SCOPE *scope;
    VALUE klass;
    NODE *cref;
    int iter;
    int vmode;
    int flags;
    int uniq;
    struct RVarmap *dyna_vars;
    VALUE orig_thread;
    VALUE wrapper;
    VALUE block_obj;
    struct BLOCK *outer;
    struct BLOCK *prev;
};

struct iter {
    int iter;
    struct iter *prev;
};


#define ITER_NOT 0
#define ITER_PRE 1
#define ITER_CUR 2

#define PUSH_ITER(i) do {		\
    struct iter _iter;			\
    _iter.prev = ruby_iter;		\
    _iter.iter = (i);			\
    ruby_iter = &_iter

#define POP_ITER()			\
    ruby_iter = _iter.prev;		\
} while (0)

#define CSTAT_PRIV  1
#define CSTAT_PROT  2
#define CSTAT_VCALL 4
#define CSTAT_SUPER 8

#define BLOCK_D_SCOPE 1
#define BLOCK_LAMBDA  2

#define PUSH_BLOCK(v,b) do {		\
    struct BLOCK _block;		\
    _block.var = (v);			\
    _block.body = (b);			\
    _block.self = self;			\
    _block.frame = *ruby_frame;		\
    _block.klass = ruby_class;		\
    _block.cref = ruby_cref;		\
    _block.frame.node = ruby_current_node;\
    _block.scope = ruby_scope;		\
    _block.prev = ruby_block;		\
    _block.outer = ruby_block;		\
    _block.iter = ruby_iter->iter;	\
    _block.vmode = scope_vmode;		\
    _block.flags = BLOCK_D_SCOPE;	\
    _block.dyna_vars = ruby_dyna_vars;	\
    _block.wrapper = ruby_wrapper;	\
    _block.block_obj = 0;		\
    _block.uniq = (b)?block_unique++:0; \
    if (b) {				\
	prot_tag->blkid = _block.uniq;  \
    }                                   \
    ruby_block = &_block

#define POP_BLOCK() \
   ruby_block = _block.prev; \
} while (0)

/* eval.c */
#if defined(HAVE_GETCONTEXT) && defined(HAVE_SETCONTEXT)

#include <signal.h>
#include <ucontext.h>
typedef struct {
    ucontext_t context;
    volatile int status;
} rb_jmpbuf_t[1];
#else
#include <setjmp.h>
typedef jmp_buf rb_jmpbuf_t;
#endif

struct tag {
    rb_jmpbuf_t buf;
    struct FRAME *frame;
    struct iter *iter;
    VALUE tag;
    VALUE retval;
    struct SCOPE *scope;
    VALUE dst;
    struct tag *prev;
    int blkid;
};

typedef struct thread * rb_thread_t;

#ifndef MAXPATHLEN
# define MAXPATHLEN 1024
#endif

struct end_proc_data {
    void (*func)();
    VALUE data;
    int safe;
    struct end_proc_data *next;
};

struct local_vars {
    ID *tbl;
    int nofree;
    int cnt;
    int dlev;
    struct RVarmap* dyna_vars;
    struct local_vars *prev;
}; 

#include <signal.h>
#ifdef __BEOS__
#undef SIGBUS
#endif

#ifndef NSIG
# ifdef DJGPP
#  define NSIG SIGMAX
# else
#  define NSIG (_SIGMAX + 1)      /* For QNX */
# endif
#endif

struct trap_list_struct {
    VALUE cmd;
    int safe;
};

#ifndef GC_MALLOC_LIMIT
#if defined(MSDOS) || defined(__human68k__)
#define GC_MALLOC_LIMIT 200000
#else
#define GC_MALLOC_LIMIT 8000000
#endif
#endif

#ifdef _WIN32
typedef LONG rb_atomic_t;
#else
typedef int rb_atomic_t;
#endif

enum thread_status {
    THREAD_TO_KILL,
    THREAD_RUNNABLE,
    THREAD_STOPPED,
    THREAD_KILLED,
    THREAD_RUNNING,
    THREAD_GC,
    THREAD_PAUSE
};
#define FOREACH_THREAD_FROM(f,x) x = f; do { x = x->next;
#define END_FOREACH_FROM(f,x) } while (x != f)

#define FOREACH_THREAD(x) FOREACH_THREAD_FROM(curr_thread,x)
#define END_FOREACH(x)    END_FOREACH_FROM(curr_thread,x)

#define FOREACH_ARY(ary, val) do { \
    int __foreach_idx; \
    VALUE val; \
    for(__foreach_idx = 0; __foreach_idx < RARRAY(ary)->len; __foreach_idx++) { \
        val = RARRAY(ary)->ptr[__foreach_idx];
#define END_FOREACH_ARY } } while(0)

/* for fd_set, etc */
#include <sys/select.h>

struct thread {
    struct thread *next, *prev;
    rb_jmpbuf_t context;
#ifdef SAVE_WIN32_EXCEPTION_LIST
    DWORD win32_exception_list;
#endif

    VALUE result;

    long   stk_len;
    long   stk_max;
    VALUE *stk_ptr;
    VALUE *stk_pos;
#ifdef __ia64__
    VALUE *bstr_ptr;
    long   bstr_len;
#endif

    struct FRAME *frame;
    struct SCOPE *scope;
    struct RVarmap *dyna_vars;
    struct BLOCK *block;
    struct iter *iter;
    struct tag *tag;
    VALUE klass;
    VALUE wrapper;
    NODE *cref;

    int flags;		/* misc. states (vmode/rb_trap_immediate/raised) */

    NODE *node;

    int thl_tracing;
    VALUE errinfo;
    VALUE last_status;
    VALUE last_line;
    VALUE last_match;

    int safe;

    enum thread_status status;
    int wait_for;
    int fd;
    fd_set readfds;
    fd_set writefds;
    fd_set exceptfds;
    int select_value;
    double delay;
    rb_thread_t join;

    int abort;
    int priority;
    VALUE thgroup;

    st_table *locals;

    VALUE thread;

    VALUE (*schedule)();
};

#define THREAD_RAISED 0x200	 /* temporary flag */
#define THREAD_TERMINATING 0x400 /* persistent flag */
#define THREAD_FLAGS_MASK  0x400 /* mask for persistent flags */


struct thread_status_t {
    NODE *node;

    int thl_tracing;
    VALUE errinfo;
    VALUE last_status;
    VALUE last_line;
    VALUE last_match;

    int safe;

    enum thread_status status;
    int wait_for;
    int fd;
    fd_set readfds;
    fd_set writefds;
    fd_set exceptfds;
    int select_value;
    double delay;
    rb_thread_t join;
};

struct gc_state {
    int dont_gc;
    int during_gc;
    int need_call_final;
    st_table *finalizer_table;
    struct gc_list *global_List;
    VALUE globals;
    RVALUE *freelist;
    RVALUE *deferred_final_list;
    struct heaps_slot *heaps;
    int heaps_length;
    int heaps_used;
    int heap_slots;
    unsigned long num_objects;
    RVALUE *gc_himem;
    RVALUE *gc_lomem;
    VALUE mark_stack[MARK_STACK_MAX];
    VALUE *mark_stack_ptr;
    int mark_stack_overflow;
    st_table *source_filenames;
    VALUE finalizers;
    unsigned long malloc_increase;
    unsigned long malloc_limit;
    int grow_direction;
    int memerror_recurse;
    
    pthread_mutex_t lock;
    pthread_t lock_owner;
    pthread_mutex_t heap_modification;
};

struct lock_table_entry {
    VALUE object;
};

typedef struct ruby_state {

    int state;
    char **environment;
    int binmode;
    VALUE signal_overriding;
    int saved_group_id;
    int saved_user_id;
    int under_uid_switch;
    int under_gid_switch;

    VALUE time_struct;

    int case_cache;
    int kcode_cache;
    int curr_kcode;
    VALUE reg_cache;
    int reg_kcode;
    int ignorecase;
    int reg_recompile_p;

    struct re_registers reg_regs;

    int uid;
    int euid;
    int gid;
    int egid;

    int original_argc;
    char **original_argv;
    int argv_len;
    char *script;

    /* from random.c */

    int rand_init;
    int rand_inc;
    int rand_left;
    int rand_initf;
    unsigned long rand_state[624]; /* FIXME 624 is N from random.c */
    unsigned long *rand_next;
    unsigned long rand_saved_seed;

    /* from pack.c */
    int pack_b64_init;
    int pack_b64_xtable[256];

    /* from ruby.c */
    VALUE progname;
    VALUE argv;
    VALUE argv0;
    VALUE ruby_debug;
    VALUE ruby_verbose;
    int sflag;
    int xflag;
    VALUE do_loop;
    VALUE do_print;
    VALUE do_check;
    VALUE do_line;
    VALUE do_split;
    VALUE init_requires;
    int end_seen;

    /* from gc.c */
    VALUE *gc_stack_start;
    st_table *rb_class_tbl;
    VALUE nomem_error;

    struct gc_state *gc;
    VALUE gc_start_hook;
    
    /* from eval.c */
    char **original_environ;
    int initialized;
    int ruby_in_compile;
    int scope_vmode;
    NODE *ruby_current_node;
    int safe_level;
    struct cache_entry cache[CACHE_SIZE];
    int cache_hits;
    int cache_misses;
    int ruby_running;
    struct FRAME initial_frame;
    struct FRAME *frames;
    struct SCOPE *ruby_scope;
    struct FRAME *top_frame;
    struct SCOPE *top_scope;
    VALUE ruby_top_self;
    unsigned long frame_unique;

    struct BLOCK *ruby_block;
    unsigned long block_unique;

    struct RVarmap *ruby_dyna_vars;
    struct iter initial_iter;
    struct iter *ruby_iter;
    struct tag *prot_tag;

    VALUE ruby_class;
    VALUE ruby_wrapper;

    NODE *ruby_cref;
    NODE *top_cref;
    rb_thread_t current_thread;
    rb_thread_t main_thread;

    VALUE trace_func;
    int tracing;

    pthread_t primary_thid;

    int ruby_in_eval;
    VALUE rb_load_path;
    VALUE ruby_dln_librefs;
    VALUE rb_features;
    st_table *loading_tbl;

    int thread_pending;
    VALUE th_raise_exception;
    NODE *th_raise_node;
    VALUE th_cmd;
    int   th_sig, th_safe;
    char *th_signm;
    int thread_init;
#if defined(_THREAD_SAFE)
    pthread_t time_thread;
#endif
    int thread_critical;
    int prohibit_interrupt;
    int exception_interrupt;
    int trap_pending;
    int trap_immediate;
    VALUE cont_protect;

    VALUE ruby_errinfo;
    VALUE ruby_errhandler;
    NODE *ruby_eval_tree_begin;
    NODE *ruby_eval_tree;
    int ruby_nerrs;

    VALUE cObject;
    VALUE cClass;
    VALUE cModule;
    VALUE output_field_seperator;
    VALUE record_seperator;
    VALUE default_record_seperator;
    VALUE output_record_seperator;
    VALUE ruby_stderr;
    VALUE ruby_stdin;
    VALUE ruby_stdout;
    VALUE ruby_deferr;
    VALUE orig_stdout;
    VALUE orig_stderr;
    VALUE fatal_exception;
    VALUE sysstack_error;
    NODE *basic_respond_to;

    int thread_abort;
    VALUE thgroup_default;

    VALUE argf;

    char *ruby_inplace_mode;
    int stack_overflowing;
    int eval_tick;
    int thread_tick;

    VALUE thread_save_value;

    rb_thread_t thread_restore;
    int thread_restore_state;
    VALUE thread_restore_value;

    /* from parse.y */
    int parse_debug;
    char *ruby_sourcefile;
    int   ruby_sourceline;
    struct local_vars *lvtbl;
    st_table *sym_tbl;
    st_table *sym_rev_tbl;
    int last_call_status;
    struct end_proc_data *end_procs, *ephemeral_end_procs, *tmp_end_procs;

    VALUE rb_last_status;

    ID last_id;
    int maxgroups;

    /* from dir.c */
    int chdir_blocking;
    VALUE chdir_thread;

    /* from dln.c */
    int dln_errno;
    int dln_init_p;
    st_table *dln_sym_tbl;
    st_table *dln_undef_tbl;
    st_table *dln_reloc_tbl;
    int link_no;
    
    int target_offset;
    char fbuf[MAXPATHLEN];
    st_table *syserr_tbl;

    /* from hash.c */
    VALUE envtbl;
    int path_tainted;

    /* from variable.c */
    st_table *rb_global_tbl;

    VALUE current_file, filename;
    int gets_lineno;
    int init_p, next_p;
    VALUE lineno;

    int special_generic_ivar;
    st_table *generic_iv_tbl;


    /* from file.c */
    VALUE separator;

    /* from string.c */
    VALUE rb_fs;

    /* from signal.c */
    struct trap_list_struct trap_list[NSIG];
    rb_atomic_t trap_pending_list[NSIG];

    # ifdef HAVE_SIGPROCMASK
    sigset_t trap_last_mask;
    # else
    int trap_last_mask;
    # endif

    pthread_mutex_t xcritical;
    pthread_t xcritical_owner;

    struct ruby_state *pristine;

    VALUE thread;
    pthread_t main_osthread;
    VALUE os_threads;
    VALUE sub_states;
    VALUE on_switch;
    VALUE interrupt;

    struct ruby_state *parent;

    struct lock_table_entry *locks;
    struct lock_table_entry *lock_entry;

    int load_depth;

    /* for behavior.c */
    int behavior_disabled;
    VALUE default_behavior;
    NODE *current_call_node;
    struct BLOCK *current_call_block;
    struct SCOPE *current_call_scope;
} ruby_state;

#define GET_THREAD(self) ((struct rb_os_thread_t*)(DATA_PTR(self)))

struct rb_os_thread_t {
    VALUE result;
    VALUE group;
    VALUE locals;
    VALUE error;
    ruby_state *state;
    pthread_t os_thread;
    enum thread_status status;
};

ruby_state *rb_get_state();
ruby_state *inherit_from_state(ruby_state*);
void rb_set_state(ruby_state *st);
ruby_state *rb_dup_state(ruby_state *state);
void init_ruby_state();
int rb_lock_xcritical();
int rb_unlock_xcritical();
void rb_state_run_interrupt();
void rb_modify();
void rb_end_modify();

#define ENTER_CRITICAL do { \
    int __thr_critical = rb_thread_critical; \
    rb_thread_critical = Qtrue; \
    rb_lock_xcritical(__FILE__,__LINE__)

#define LEAVE_CRITICAL \
    rb_thread_critical = __thr_critical; \
    rb_unlock_xcritical(); } while(0)

#ifdef GC_DEBUG

#define GC_LOCK fprintf(stderr, "locked at %s:%d\n", __FILE__, __LINE__); pthread_mutex_lock(&GC_VAR(lock)); \
    GC_VAR(lock_owner) = pthread_self()

#define GC_UNLOCK pthread_mutex_unlock(&GC_VAR(lock)); fprintf(stderr, "unlocked at %s:%d\n", __FILE__, __LINE__)

#define HEAP_LOCK fprintf(stderr, "heap locked at %s:%d\n", __FILE__, __LINE__); pthread_mutex_lock(&GC_VAR(heap_modification));
#define HEAP_UNLOCK pthread_mutex_unlock(&GC_VAR(heap_modification)); fprintf(stderr, "heap unlocked at %s:%d\n", __FILE__, __LINE__)

#else

#if 0

#define GC_LOCK if(!pthread_equal(pthread_self(), GC_VAR(lock_owner))) { \
    pthread_mutex_lock(&GC_VAR(lock)); GC_VAR(lock_owner) = pthread_self(); }

#define GC_UNLOCK if(!pthread_equal(pthread_self(), GC_VAR(lock_owner))) pthread_mutex_unlock(&GC_VAR(lock))

#endif

#define GC_LOCK pthread_mutex_lock(&GC_VAR(lock))

#define GC_UNLOCK pthread_mutex_unlock(&GC_VAR(lock))

#define HEAP_LOCK pthread_mutex_lock(&GC_VAR(heap_modification))
#define HEAP_UNLOCK pthread_mutex_unlock(&GC_VAR(heap_modification))

#endif

#define MODIFY(value) rb_modify(value)

#define END_MODIFY(value) rb_end_modify(value)
void __place_exit(char*, int, int);

#define RUBY_STATE ((ruby_state*)rb_get_state())
#define STATE_VAR(obj) (RUBY_STATE->obj)
#define GC_VAR(obj) STATE_VAR(gc->obj)
// #define rb_cObject STATE_VAR(cObject)
// #define rb_cClass  STATE_VAR(cClass)

#endif

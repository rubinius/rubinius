
struct METHOD {
    VALUE klass, rklass;
    VALUE recv;
    ID id, oid;
    NODE *body;
};

enum lex_state {
    EXPR_BEG,			/* ignore newline, +/- is a sign. */
    EXPR_END,			/* newline significant, +/- is a operator. */
    EXPR_ARG,			/* newline significant, +/- is a operator. */
    EXPR_CMDARG,		/* newline significant, +/- is a operator. */
    EXPR_ENDARG,		/* newline significant, +/- is a operator. */
    EXPR_MID,			/* newline significant, +/- is a operator. */
    EXPR_FNAME,			/* ignore newline, no reserved words. */
    EXPR_DOT,			/* right after `.' or `::', no reserved words. */
    EXPR_CLASS,			/* immediate after `class', no here document. */
}; 

#ifdef HAVE_LONG_LONG
typedef unsigned LONG_LONG stack_type;
#else
typedef unsigned long stack_type;
#endif

typedef struct rb_parse_state {
    int end_seen;
    VALUE debug_lines;
    int heredoc_end;
    int command_start;
    NODE *lex_strterm;
    int class_nest;
    int in_single;
    int in_def;
    int compile_for_eval;
    ID cur_mid;
    char *token_buffer;
    int tokidx;
    int toksiz;

    VALUE (*lex_gets)();	/* gets function */
    VALUE lex_input;		/* non-nil if File */
    VALUE lex_lastline;	/* gc protect */
    char *lex_pbeg;
    char *lex_p;
    char *lex_pend;
    int lex_gets_ptr;

    enum lex_state lex_state;
    int in_defined;
    stack_type cond_stack;
    stack_type cmdarg_stack;

    void *lval; /* the parser's yylval */

    VALUE comments;
    int column;
    NODE *top;
    ID *locals;
    
    VALUE self;

    int ternary_colon;
    
} rb_parse_state;

rb_parse_state *alloc_parse_state();

#define PARSE_STATE ((rb_parse_state*)parse_state)
#define PARSE_VAR(var) (PARSE_STATE->var)

#define PROT_NONE   Qfalse	/* 0 */
#define PROT_THREAD Qtrue	/* 2 */
#define PROT_FUNC   INT2FIX(0)	/* 1 */
#define PROT_LOOP   INT2FIX(1)	/* 3 */
#define PROT_LAMBDA INT2FIX(2)	/* 5 */
#define PROT_YIELD  INT2FIX(3)	/* 7 */

#define EXEC_TAG()    (FLUSH_REGISTER_WINDOWS, setjmp(prot_tag->buf))
#define RUN_TAG(var)  if((var = EXEC_TAG()) == 0)

#define JUMP_TAG(st) do {		\
    ruby_frame = prot_tag->frame;	\
    ruby_iter = prot_tag->iter;		\
    longjmp(prot_tag->buf,(st));	\
} while (0)

#define POP_TAG()			\
    prot_tag = _tag.prev;		\
} while (0)

#define TAG_DST() (_tag.dst == (VALUE)ruby_frame->uniq)

#define TAG_RETURN	0x1
#define TAG_BREAK	0x2
#define TAG_NEXT	0x3
#define TAG_RETRY	0x4
#define TAG_REDO	0x5
#define TAG_RAISE	0x6
#define TAG_THROW	0x7
#define TAG_FATAL	0x8
#define TAG_MASK	0xf

#define DVAR_DONT_RECYCLE FL_USER2

#define PUSH_VARS() do { \
    struct RVarmap * volatile _old; \
    _old = ruby_dyna_vars; \
    ruby_dyna_vars = 0

#define POP_VARS() \
    if (_old && (ruby_scope->flags & SCOPE_DONT_RECYCLE)) {\
	if (RBASIC(_old)->flags) /* unless it's already recycled */ \
	    FL_SET(_old, DVAR_DONT_RECYCLE); \
    }\
    ruby_dyna_vars = _old; \
} while (0)

#define PUSH_CLASS(c) do {		\
    VALUE _class = ruby_class;		\
    ruby_class = (c)

#define POP_CLASS() ruby_class = _class; \
} while (0)

VALUE rb_yield_0 _((VALUE, VALUE, VALUE, int, int));

#define YIELD_LAMBDA_CALL 1
#define YIELD_PROC_CALL   2
#define YIELD_PUBLIC_DEF  4
#define YIELD_FUNC_AVALUE 1
#define YIELD_FUNC_SVALUE 2

#define SCOPE_PUBLIC    0
#define SCOPE_PRIVATE   1
#define SCOPE_PROTECTED 2
#define SCOPE_MODFUNC   5
#define SCOPE_MASK      7
#define SCOPE_SET(f)  (scope_vmode=(f))
#define SCOPE_TEST(f) (scope_vmode&(f))

#define PUSH_SCOPE() do {		\
    volatile int _vmode = scope_vmode;	\
    struct SCOPE * volatile _old;	\
    NEWOBJ(_scope, struct SCOPE);	\
    OBJSETUP(_scope, 0, T_SCOPE);	\
    _scope->local_tbl = 0;		\
    _scope->local_vars = 0;		\
    _scope->flags = 0;			\
    _old = ruby_scope;			\
    ruby_scope = _scope;		\
    ruby_frame->scope = ruby_scope;     \
    scope_vmode = SCOPE_PUBLIC

#define POP_SCOPE() 			\
    if (ruby_scope->flags & SCOPE_DONT_RECYCLE) {\
	if (_old) scope_dup(_old);	\
    }					\
    if (!(ruby_scope->flags & SCOPE_MALLOC)) {\
	ruby_scope->local_vars = 0;	\
	ruby_scope->local_tbl  = 0;	\
	if (!(ruby_scope->flags & SCOPE_DONT_RECYCLE) && \
	    ruby_scope != top_scope) {	\
	    rb_gc_force_recycle((VALUE)ruby_scope);\
	}				\
    }					\
    ruby_scope->flags |= SCOPE_NOSTACK;	\
    ruby_scope = _old;			\
    scope_vmode = _vmode;		\
} while (0)

#define PUSH_FRAME() do {		\
    struct FRAME _frame;		\
    _frame.prev = ruby_frame;		\
    _frame.tmp  = 0;			\
    _frame.node = ruby_current_node;	\
    _frame.iter = ruby_iter->iter;	\
    _frame.argc = 0;			\
    _frame.argv = 0;			\
    _frame.orig_func = 0;               \
    _frame.flags = FRAME_ALLOCA;	\
    _frame.uniq = frame_unique++;	\
    _frame.metadata = Qnil;             \
    _frame.scope = ruby_scope;          \
    ruby_frame = &_frame

#define POP_FRAME()  			\
    ruby_current_node = _frame.node;	\
    ruby_frame = _frame.prev;		\
} while (0)

#define PUSH_CREF(c) ruby_cref = NEW_NODE(NODE_CREF,(c),0,ruby_cref)
#define POP_CREF() ruby_cref = ruby_cref->nd_next


#ifndef __GRAMMER_RUNTIME_H__
#define __GRAMMER_RUNTIME_H__
rb_parse_state *alloc_parse_state();
void *pt_allocate(rb_parse_state *st, int size);
void pt_free(rb_parse_state *st);
void create_error(rb_parse_state *parse_state, char *msg);

#endif /* __GRAMMER_RUNTIME_H__ */

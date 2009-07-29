#ifndef MEL_VISITOR_HPP
#define MEL_VISITOR_HPP

#ifdef __cplusplus
extern "C" {
#endif

namespace melbourne {
  rb_parse_state *alloc_parse_state();
  void *pt_allocate(rb_parse_state *st, int size);
  void pt_free(rb_parse_state *st);
  void create_error(rb_parse_state *parse_state, char *msg);
  NODE *node_newnode(rb_parse_state*, enum node_type, VALUE, VALUE, VALUE);
};

#ifdef __cplusplus
}  /* extern "C" { */
#endif

#endif

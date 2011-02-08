#ifndef MEL_VISITOR_HPP
#define MEL_VISITOR_HPP

namespace melbourne {
  rb_parser_state *alloc_parser_state();
  void *pt_allocate(rb_parser_state *st, int size);
  void pt_free(rb_parser_state *st);

  namespace grammar18 {
    void create_error(rb_parser_state *parser_state, char *msg);
    NODE *parser_node_newnode(rb_parser_state*, enum node_type, VALUE, VALUE, VALUE);
  };

  namespace grammar19 {
    void create_error(rb_parser_state *parser_state, char *msg);
    NODE *parser_node_newnode(rb_parser_state*, enum node_type, VALUE, VALUE, VALUE);
  };

#define node_newnode(t, a, b, c)  parser_node_newnode((rb_parser_state*)parser_state, t, a, b, c)
};

#endif

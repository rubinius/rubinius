#ifndef RBX_GRAMMAR_RUNTIME_HPP
#define RBX_GRAMMAR_RUNTIME_HPP

namespace rubinius {
  namespace parser {
    rb_parse_state *alloc_parse_state();
    void *pt_allocate(rb_parse_state *st, int size);
    void pt_free(rb_parse_state *st);
    void create_error(rb_parse_state *parse_state, char *msg);
    NODE *syd_node_newnode(rb_parse_state*, enum node_type, Object*, Object*, Object*);
  };
};

#endif

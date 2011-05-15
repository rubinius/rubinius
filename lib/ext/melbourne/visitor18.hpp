#ifndef MEL_VISITOR18_HPP
#define MEL_VISITOR18_HPP

namespace melbourne {
  namespace grammar18 {
    void create_error(rb_parser_state *parser_state, char *msg);
    NODE *parser_node_newnode(rb_parser_state*, enum node_type, VALUE, VALUE, VALUE);
    VALUE process_parse_tree(rb_parser_state*, VALUE, NODE*, QUID*);
  };
};

#endif

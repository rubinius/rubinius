str_nodes = %w!block defined colon2 match2 match3 begin opt_n not if case when while
  until block_pass retry false nil self true zarray zsuper redo
  break next return super method splat to_ary svalue dsym evstr
  scope yield rescue resbody ensure dot2 dot3 flip2 flip3 argscat
  argspush op_asgn_and op_asgn_or and or call fcall vcall op_asgn1
  op_asgn2 masgn lasgn iasgn dasgn dasgn_curr cvasgn cvdecl gasgn
  cdecl alias valias hash array dstr dxstr dregx defs defn class
  module sclass args lvar dvar ivar cvar gvar const attrset
  str xstr match lit newline nth_ref back_ref block_arg attrasgn
  iter for!

puts "static st_table *_id_tbl;"

str_nodes.each do |node|
  puts "METH(#{node});"
end

puts "static inline NODE *_dispatch(ID curr, VALUE ary) {"
puts "  int idx = -1;"
puts '  if(!st_lookup(_id_tbl, (st_data_t)curr, &idx)) { printf("Unable to find value \'%s\'!\n", rb_id2name(curr)); rb_p(ary); abort(); }'
puts "  switch(idx) {"
str_nodes.each_with_index do |node, idx|
  puts "  case #{idx}:"
  puts "     return CALL(#{node}, ary);"
end

puts "  default:"
puts '      printf("Unknown node type %d, %d, %s!\n", idx, curr, rb_id2name(curr));'
puts '      return NULL;'
puts "   }"
puts "}"

puts "static _init_dispatch() {"
puts "   _id_tbl = st_init_numtable_with_size(#{str_nodes.size});"
str_nodes.each_with_index do |node, idx|
  puts "   st_insert(_id_tbl, (st_data_t)rb_intern(#{node.dump}), (st_data_t)#{idx});"
end
puts "}"

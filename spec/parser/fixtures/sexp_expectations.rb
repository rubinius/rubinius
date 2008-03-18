SEXP_EXPECTATIONS = {
  "alias"  => {
    "Ruby"        => "class X\n  alias :y :x\nend",
    "ParseTree"   => [:class, :X, nil,
                      [:scope, [:alias, [:lit, :y], [:lit, :x]]]],
    "Ruby2Ruby"   => "class X\n  alias_method :y, :x\nend",
  },

  "alias_ugh"  => {
    "Ruby"        => "class X\n  alias y x\nend",
    "ParseTree"   => [:class, :X, nil,
                      [:scope, [:alias, [:lit, :y], [:lit, :x]]]],
    "Ruby2Ruby"   => "class X\n  alias_method :y, :x\nend",
  },

  "and"  => {
    "Ruby"        => "(a and b)",
    "ParseTree"   => [:and, [:vcall, :a], [:vcall, :b]],
  },

  "argscat_inside"  => {
    "Ruby"        => "a = [b, *c]",
    "ParseTree"   => [:lasgn, :a,
                      [:argscat, [:array, [:vcall, :b]], [:vcall, :c]]],
    "Ruby2Ruby"   => "a = b, *c",
  },

  "argscat_svalue"  => {
    "Ruby"        => "a = b, c, *d",
    "ParseTree"   => [:lasgn, :a,
                      [:svalue,
                       [:argscat,
                        [:array, [:vcall, :b], [:vcall, :c]],
                        [:vcall, :d]]]],
  },

  "argspush"  => {
    "Ruby"        => "a[*b] = c",
    "ParseTree"   => [:attrasgn,
                      [:vcall, :a],
                      :[]=,
                      [:argspush, [:splat, [:vcall, :b]], [:vcall, :c]]],
  },

  "array"  => {
    "Ruby"        => "[1, :b, \"c\"]",
    "ParseTree"   => [:array, [:lit, 1], [:lit, :b], [:str, "c"]],
  },

  "array_pct_W"  => {
    "Ruby"        => "%W[--remove #\{@gem_repo}]",
    "ParseTree"   => [:array,
                      [:str, "--remove"],
                      [:dstr, "", [:evstr, [:ivar, :@gem_repo]]]],
    "Ruby2Ruby"   => "[\"--remove\", \"#\{@gem_repo}\"]",
  },

  "attrasgn" => {
    "Ruby"        => "y = 0\n42.method = y\n",
    "ParseTree"   => [:block,
                      [:lasgn, :y, [:lit, 0]],
                      [:attrasgn, [:lit, 42], :method=, [:array, [:lvar, :y]]]],
  },

  "attrasgn_index_equals" => {
    "Ruby"        => "a[42] = 24",
    "ParseTree"   => [:attrasgn, [:vcall, :a], :[]=, [:array, [:lit, 42], [:lit, 24]]],
  },

#   "attrset" => {
#     "Ruby"        => [Examples, :writer=],
#     "ParseTree"   => [:defn, :writer=, [:attrset, :@writer]],
#     "Ruby2Ruby"   => "attr_writer :writer"
#   },

  "back_ref"  => {
    "Ruby"        => "[$&, $`, $', $+]",
    "ParseTree"   => [:array,
                      [:back_ref, :&],
                      [:back_ref, :"`"],
                      [:back_ref, :"'"],
                      [:back_ref, :+]],
  },

  "begin"  => {
    "Ruby"        => "begin\n  (1 + 1)\nend",
    "ParseTree"   => [:begin, [:call, [:lit, 1], :+, [:array, [:lit, 1]]]],
  },

  "begin_def"  => {
    "Ruby"        => "def m\n  begin\n\n  end\nend",
    "ParseTree"   => [:defn, :m, [:scope, [:block, [:args], [:nil]]]],
    "Ruby2Ruby"   => "def m\n  # do nothing\nend",
  },

  "begin_rescue_ensure" => {
    "Ruby" => "begin\nrescue\n  # do nothing\nensure\n  nil\nend",
    "ParseTree" => [:begin,
                    [:ensure,
                     [:rescue,
                      [:resbody, nil]],
                     [:nil]]]
  },

  "begin_rescue_twice" => { # testing block/begin processing really
    "Ruby" => "begin\nrescue => mes\n  # do nothing\nend\nbegin\nrescue => mes\n  # do nothing\nend\n",
    "ParseTree" => [:block,
                    [:rescue,
                     [:resbody, nil,
                      [:lasgn, :mes, [:gvar, :$!]]]],
                    [:begin,
                     [:rescue,
                      [:resbody, nil,
                       [:lasgn, :mes, [:gvar, :$!]]]]]],
  },

  "block_lasgn" => {
    "Ruby"        => "x = (y = 1\n(y + 2))",
    "ParseTree"   => [:lasgn, :x,
                      [:block,
                       [:lasgn, :y, [:lit, 1]],
                       [:call, [:lvar, :y], :+, [:array, [:lit, 2]]]]],
  },

  "block_mystery_block"  => { # FIX: I don't like that extra return
    "Ruby"        => "a(b) do\n  if b then\n    true\n  else\n    c = false\n    d { |x| c = true }\n    c\n  \n  end\nend",
    "ParseTree"   => [:iter,
                      [:fcall, :a, [:array, [:vcall, :b]]],
                      nil,
                      [:block,
                       [:dasgn_curr, :c],
                       [:if,
                        [:vcall, :b],
                        [:true],
                        [:block,
                         [:dasgn_curr, :c, [:false]],
                         [:iter,
                          [:fcall, :d],
                          [:dasgn_curr, :x],
                          [:dasgn, :c, [:true]]],
                         [:dvar, :c]]]]],
  },

  "block_pass_args_and_splat" => {
    "Ruby" => "def blah(*args, &block)\n  other(42, *args, &block)\nend",
    "ParseTree" => [:defn, :blah,
                    [:scope,
                     [:block,
                      [:args, :"*args"],
                      [:block_arg, :block],
                      [:block_pass,
                       [:lvar, :block],
                       [:fcall, :other,
                        [:argscat, [:array, [:lit, 42]], [:lvar, :args]]]]]]],
  },

  "block_pass_call_0"  => {
    "Ruby"        => "a.b(&c)",
    "ParseTree"   => [:block_pass, [:vcall, :c], [:call, [:vcall, :a], :b]],
  },

  "block_pass_call_1"  => {
    "Ruby"        => "a.b(4, &c)",
    "ParseTree"   => [:block_pass,
                      [:vcall, :c],
                      [:call, [:vcall, :a], :b, [:array, [:lit, 4]]]],
  },

  "block_pass_call_n"  => {
    "Ruby"        => "a.b(1, 2, 3, &c)",
    "ParseTree"   => [:block_pass,
                      [:vcall, :c],
                      [:call, [:vcall, :a], :b,
                       [:array, [:lit, 1], [:lit, 2], [:lit, 3]]]],
  },

  "block_pass_fcall_0"  => {
    "Ruby"        => "a(&b)",
    "ParseTree"   => [:block_pass, [:vcall, :b], [:fcall, :a]],
  },

  "block_pass_fcall_1"  => {
    "Ruby"        => "a(4, &b)",
    "ParseTree"   => [:block_pass,
                      [:vcall, :b],
                      [:fcall, :a, [:array, [:lit, 4]]]],
  },

  "block_pass_fcall_n"  => {
    "Ruby"        => "a(1, 2, 3, &b)",
    "ParseTree"   => [:block_pass,
                      [:vcall, :b],
                      [:fcall, :a,
                       [:array, [:lit, 1], [:lit, 2], [:lit, 3]]]],
  },

  "block_pass_omgwtf" => {
    "Ruby" => "define_attr_method(:x, :sequence_name, &Proc.new { |*args| nil })",
    "ParseTree" => [:block_pass,
                    [:iter,
                     [:call, [:const, :Proc], :new],
                     [:masgn, [:dasgn_curr, :args]],
                     [:nil]],
                    [:fcall, :define_attr_method,
                     [:array, [:lit, :x], [:lit, :sequence_name]]]],
  },

  "block_pass_splat" => {
    "Ruby" => "def blah(*args, &block)\n  other(*args, &block)\nend",
    "ParseTree" => [:defn, :blah,
                    [:scope,
                     [:block,
                      [:args, :"*args"],
                      [:block_arg, :block],
                      [:block_pass,
                       [:lvar, :block],
                       [:fcall, :other, [:splat, [:lvar, :args]]]]]]],
  },

  "block_pass_thingy" => {
    "Ruby"      => "r.read_body(dest, &block)",
    "ParseTree" => [:block_pass,
                    [:vcall, :block],
                    [:call, [:vcall, :r], :read_body,
                     [:array, [:vcall, :dest]]]],
  },

  "block_stmt_after"  => {
    "Ruby"        => "def f\n  begin\n    b\n  rescue\n    c\n  end\n\n  d\nend",
    "ParseTree"   => [:defn, :f,
                      [:scope,
                       [:block,
                        [:args],
                        [:rescue, [:vcall, :b], [:resbody, nil, [:vcall, :c]]],
                        [:vcall, :d]]]],
    "Ruby2Ruby"   => "def f\n  b rescue c\n  d\nend",
  },

  "block_stmt_before"  => {
    "Ruby"        => "def f\n  a\n  begin\n    b\n  rescue\n    c\n  end\nend",
    "ParseTree"   => [:defn, :f,
                      [:scope,
                       [:block,
                        [:args],
                        [:vcall, :a],
                        [:begin,
                         [:rescue, [:vcall, :b],
                          [:resbody, nil, [:vcall, :c]]]]]]],
  },

  "block_stmt_both"  => {
    "Ruby"        => "def f\n  a\n  begin\n    b\n  rescue\n    c\n  end\n  d\nend",
    "ParseTree"   => [:defn, :f,
                      [:scope,
                       [:block,
                        [:args],
                        [:vcall, :a],
                        [:rescue, [:vcall, :b], [:resbody, nil, [:vcall, :c]]],
                        [:vcall, :d]]]],
    "Ruby2Ruby"   => "def f\n  a\n  b rescue c\n  d\nend",
  },

#   "bmethod"  => {
#     "Ruby"        => [Examples, :unsplatted],
#     "ParseTree"   => [:defn, :unsplatted,
#                       [:bmethod,
#                        [:dasgn_curr, :x],
#                        [:call, [:dvar, :x], :+, [:array, [:lit, 1]]]]],
#     "Ruby2Ruby"   => "def unsplatted(x)\n  (x + 1)\nend"
#   },

#   "bmethod_noargs"  => {
#     "Ruby"        => [Examples, :bmethod_noargs],
#     "ParseTree"   => [:defn, :bmethod_noargs,
#                       [:bmethod,
#                        nil,
#                        [:call, [:vcall, :x], :"+", [:array, [:lit, 1]]]]],
#     "Ruby2Ruby"   => "def bmethod_noargs\n  (x + 1)\nend"
#   },

#   "bmethod_splat" => {
#     "Ruby" => [Examples, :splatted],
#     "ParseTree" => [:defn, :splatted,
#                     [:bmethod,
#                      [:masgn, [:dasgn_curr, :args]],
#                      [:block,
#                       [:dasgn_curr, :y, [:call, [:dvar, :args], :first]],
#                       [:call, [:dvar, :y], :+, [:array, [:lit, 42]]]]]],
#     "Ruby2Ruby" => "def splatted(*args)\n  y = args.first\n  (y + 42)\nend",
#   },

  "break"  => {
    "Ruby"        => "loop { break if true }",
    "ParseTree"   => [:iter,
                      [:fcall, :loop], nil, [:if, [:true], [:break], nil]],
  },

  "break_arg"  => {
    "Ruby"        => "loop { break 42 if true }",
    "ParseTree"   => [:iter,
                      [:fcall, :loop], nil,
                      [:if, [:true], [:break, [:lit, 42]], nil]],
  },

  "call" => {
    "Ruby"        => "self.method",
    "ParseTree" => [:call, [:self], :method],
  },

  "call_arglist"  => {
    "Ruby"        => "o.puts(42)",
    "ParseTree"   => [:call, [:vcall, :o], :puts, [:array, [:lit, 42]]],
  },

  "call_arglist_hash"  => {
    "Ruby"        => "o.m(:a => 1, :b => 2)",
    "ParseTree"   => [:call,
                      [:vcall, :o], :m,
                      [:array,
                       [:hash, [:lit, :a], [:lit, 1], [:lit, :b], [:lit, 2]]]],
  },

  "call_arglist_norm_hash"  => {
    "Ruby"        => "o.m(42, :a => 1, :b => 2)",
    "ParseTree"   => [:call,
                      [:vcall, :o], :m,
                      [:array,
                       [:lit, 42],
                       [:hash, [:lit, :a], [:lit, 1], [:lit, :b], [:lit, 2]]]],
  },

  "call_arglist_norm_hash_splat"  => {
    "Ruby"        => "o.m(42, :a => 1, :b => 2, *c)",
    "ParseTree"   => [:call,
                      [:vcall, :o], :m,
                      [:argscat,
                       [:array,
                        [:lit, 42],
                        [:hash, [:lit, :a], [:lit, 1], [:lit, :b], [:lit, 2]]],
                       [:vcall, :c]]],
  },

  "call_command" => {
    "Ruby"        => "1.b(c)",
    "ParseTree"   => [:call, [:lit, 1], :b, [:array, [:vcall, :c]]],
  },

  "call_expr" => {
    "Ruby"        => "(v = (1 + 1)).zero?",
    "ParseTree"   => [:call,
                      [:lasgn, :v,
                       [:call, [:lit, 1], :+, [:array, [:lit, 1]]]],
                      :zero?],
  },

  "call_index" => { # see attrasgn_index_equals for opposite
    "Ruby"        => "a[42]",
    "ParseTree"   => [:call, [:vcall, :a], :[], [:array, [:lit, 42]]],
  },

  "call_index_no_args" => {
    "Ruby"        => "a[]",
    "ParseTree" => [:call, [:vcall, :a], :[]],
  },

  "call_unary_neg" => {
    "Ruby"        => "-2**31",
    "ParseTree"   => [:call,
                      [:call, [:lit, 2], :**, [:array, [:lit, 31]]],
                      :-@],
    "Ruby2Ruby"   => "-(2 ** 31)",
  },

  "case" => {
    "Ruby"        => "var = 2\nresult = \"\"\ncase var\nwhen 1 then\n  puts(\"something\")\n  result = \"red\"\nwhen 2, 3 then\n  result = \"yellow\"\nwhen 4 then\n  # do nothing\nelse\n  result = \"green\"\nend\ncase result\nwhen \"red\" then\n  var = 1\nwhen \"yellow\" then\n  var = 2\nwhen \"green\" then\n  var = 3\nelse\n  # do nothing\nend\n",
    "ParseTree" => [:block,
                    [:lasgn, :var, [:lit, 2]],
                    [:lasgn, :result, [:str, ""]],
                    [:case,
                     [:lvar, :var],
                     [:when,
                      [:array, [:lit, 1]],
                      [:block,
                       [:fcall, :puts, [:array, [:str, "something"]]],
                       [:lasgn, :result, [:str, "red"]]]],
                     [:when,
                      [:array, [:lit, 2], [:lit, 3]],
                      [:lasgn, :result, [:str, "yellow"]]],
                     [:when, [:array, [:lit, 4]], nil],
                     [:lasgn, :result, [:str, "green"]]],
                    [:case,
                     [:lvar, :result],
                     [:when, [:array, [:str, "red"]],
                      [:lasgn, :var, [:lit, 1]]],
                     [:when, [:array, [:str, "yellow"]],
                      [:lasgn, :var, [:lit, 2]]],
                     [:when, [:array, [:str, "green"]],
                      [:lasgn, :var, [:lit, 3]]],
                     nil]]
  },

  "case_nested" => {
    "Ruby"        => "var1 = 1\nvar2 = 2\nresult = nil\ncase var1\nwhen 1 then\n  case var2\n  when 1 then\n    result = 1\n  when 2 then\n    result = 2\n  else\n    result = 3\n  end\nwhen 2 then\n  case var2\n  when 1 then\n    result = 4\n  when 2 then\n    result = 5\n  else\n    result = 6\n  end\nelse\n  result = 7\nend\n",
    "ParseTree" => [:block,
                    [:lasgn, :var1, [:lit, 1]],
                    [:lasgn, :var2, [:lit, 2]],
                    [:lasgn, :result, [:nil]],
                    [:case,
                     [:lvar, :var1],
                     [:when, [:array, [:lit, 1]],
                      [:case,
                       [:lvar, :var2],
                       [:when, [:array, [:lit, 1]],
                        [:lasgn, :result, [:lit, 1]]],
                       [:when, [:array, [:lit, 2]],
                        [:lasgn, :result, [:lit, 2]]],
                       [:lasgn, :result, [:lit, 3]]]],
                     [:when, [:array, [:lit, 2]],
                      [:case,
                       [:lvar, :var2],
                       [:when, [:array, [:lit, 1]],
                        [:lasgn, :result, [:lit, 4]]],
                       [:when, [:array, [:lit, 2]],
                        [:lasgn, :result, [:lit, 5]]],
                       [:lasgn, :result, [:lit, 6]]]],
                     [:lasgn, :result, [:lit, 7]]]]
  },

  "case_nested_inner_no_expr"  => {
    "Ruby"        => "case a\nwhen b then\n  case\n  when (d and e) then\n    f\n  else\n    # do nothing\n  end\nelse\n  # do nothing\nend",
    "ParseTree"   => [:case, [:vcall, :a],
                      [:when, [:array, [:vcall, :b]],
                       [:case, nil,
                        [:when, [:array, [:and, [:vcall, :d], [:vcall, :e]]],
                         [:vcall, :f]],
                        nil]],
                      nil],
  },

  "case_no_expr" => { # TODO: nested
    "Ruby" => "case\nwhen (a == 1) then\n  :a\nwhen (a == 2) then\n  :b\nelse\n  :c\nend",
    "ParseTree" => [:case, nil,
                    [:when,
                     [:array, [:call, [:vcall, :a], :==, [:array, [:lit, 1]]]],
                     [:lit, :a]],
                    [:when,
                     [:array, [:call, [:vcall, :a], :==, [:array, [:lit, 2]]]],
                     [:lit, :b]],
                    [:lit, :c]],
  },

  "case_splat" => {
    "Ruby" => "case a\nwhen :b, *c then\n  d\nelse\n  e\nend",
    "ParseTree" => [:case, [:vcall, :a],
                    [:when,
                     [:array, [:lit, :b], [:when, [:vcall, :c], nil]], # wtf?
                     [:vcall, :d]],
                    [:vcall, :e]],
  },

  "cdecl"  => {
    "Ruby"        => "X = 42",
    "ParseTree"   => [:cdecl, :X, [:lit, 42]],
  },

  "class_plain"  => {
    "Ruby"        => "class X\n  puts((1 + 1))\n  def blah\n    puts(\"hello\")\n  end\nend",
    "ParseTree"   => [:class,
                      :X,
                      nil,
                      [:scope,
                       [:block,
                        [:fcall, :puts, [:array, [:call, [:lit, 1], :+, [:array, [:lit, 1]]]]],
                        [:defn, :blah,
                         [:scope,
                          [:block,
                           [:args],
                           [:fcall, :puts, [:array, [:str, "hello"]]]]]]]]],
  },

  "class_super_array"  => {
    "Ruby"        => "class X < Array\nend",
    "ParseTree"   => [:class,
                      :X,
                      [:const, :Array],
                      [:scope]],
  },

  "class_super_expr"  => {
    "Ruby"        => "class X < expr\nend",
    "ParseTree"   => [:class,
                      :X,
                      [:vcall, :expr],
                      [:scope]],
  },

  "class_super_object"  => {
    "Ruby"        => "class X < Object\nend",
    "ParseTree"   => [:class,
                      :X,
                      [:const, :Object],
                      [:scope]],
  },

  "colon2"  => {
    "Ruby"        => "X::Y",
    "ParseTree"   => [:colon2, [:const, :X], :Y],
  },

  "colon3"  => {
    "Ruby"        => "::X",
    "ParseTree"   => [:colon3, :X],
  },

  "conditional1" => { # TODO: rename
    "Ruby"        => "return 1 if (42 == 0)",
    "ParseTree"   => [:if,
                      [:call, [:lit, 42], :==, [:array, [:lit, 0]]],
                      [:return, [:lit, 1]],
                      nil],
  },

  "conditional2" => { # TODO: remove
    "Ruby"        => "return 2 unless (42 == 0)",
    "ParseTree"   => [:if,
                      [:call, [:lit, 42], :==, [:array, [:lit, 0]]],
                      nil,
                      [:return, [:lit, 2]]],
  },

  "conditional3" => {
    "Ruby"        => "if (42 == 0) then\n  return 3\nelse\n  return 4\nend",
    "ParseTree"   => [:if, [:call, [:lit, 42], :==, [:array, [:lit, 0]]],
                      [:return, [:lit, 3]],
                      [:return, [:lit, 4]]],
  },

  "conditional4" => {
    "Ruby"        => "if (42 == 0) then\n  return 2\nelsif (42 < 0) then\n  return 3\nelse\n  return 4\nend",
    "ParseTree"   => [:if,
                      [:call, [:lit, 42], :==, [:array, [:lit, 0]]],
                      [:return, [:lit, 2]],
                      [:if,
                       [:call, [:lit, 42], :<, [:array, [:lit, 0]]],
                       [:return, [:lit, 3]],
                       [:return, [:lit, 4]]]],
    "Ruby2Ruby"   => "if (42 == 0) then\n  return 2\nelse\n  if (42 < 0) then\n    return 3\n  else\n    return 4\n  end\nend",
  },

  "conditional5" => {
    "Ruby"       => "return if false unless true",
    "ParseTree"  => [:if, [:true], nil, [:if, [:false], [:return], nil]],
  },

  "conditional_post_if"  => {
    "Ruby"        => "a if b",
    "ParseTree"   => [:if, [:vcall, :b], [:vcall, :a], nil],
  },

  "conditional_post_if_not"  => {
    "Ruby"        => "a if not b",
    "ParseTree"   => [:if, [:vcall, :b], nil, [:vcall, :a]],
    "Ruby2Ruby"   => "a unless b"
  },

  "conditional_post_unless"  => {
    "Ruby"        => "a unless b",
    "ParseTree"   => [:if, [:vcall, :b], nil, [:vcall, :a]],
  },

  "conditional_post_unless_not"  => {
    "Ruby"        => "a unless not b",
    "ParseTree"   => [:if, [:vcall, :b], [:vcall, :a], nil],
    "Ruby2Ruby"   => "a if b"
  },

  "conditional_pre_if"  => {
    "Ruby"        => "if b then a end",
    "ParseTree"   => [:if, [:vcall, :b], [:vcall, :a], nil],
    "Ruby2Ruby"   => "a if b"
  },

  "conditional_pre_if_not"  => {
    "Ruby"        => "if not b then a end",
    "ParseTree"   => [:if, [:vcall, :b], nil, [:vcall, :a]],
    "Ruby2Ruby"   => "a unless b"
  },

  "conditional_pre_unless"  => {
    "Ruby"        => "unless b then a end",
    "ParseTree"   => [:if, [:vcall, :b], nil, [:vcall, :a]],
    "Ruby2Ruby"   => "a unless b"
  },

  "conditional_pre_unless_not"  => {
    "Ruby"        => "unless not b then a end",
    "ParseTree"   => [:if, [:vcall, :b], [:vcall, :a], nil],
    "Ruby2Ruby"   => "a if b"
  },

  "const"  => {
    "Ruby"        => "X",
    "ParseTree"   => [:const, :X],
  },

  "cvar"  => {
    "Ruby"        => "@@x",
    "ParseTree"   => [:cvar, :@@x],
  },

  "cvasgn"  => {
    "Ruby"        => "def x\n  @@blah = 1\nend",
    "ParseTree"   => [:defn, :x,
                      [:scope,
                       [:block, [:args], [:cvasgn, :@@blah, [:lit, 1]]]]]
  },

  "cvasgn_cls_method" => {
    "Ruby"        => "def self.quiet_mode=(boolean)\n  @@quiet_mode = boolean\nend",
    "ParseTree"   => [:defs, [:self], :quiet_mode=, [:scope, [:block, [:args, :boolean], [:cvasgn, :@@quiet_mode, [:lvar, :boolean]]]]],
  },

  "cvdecl"  => {
    "Ruby"        => "class X\n  @@blah = 1\nend",
    "ParseTree"   => [:class, :X, nil,
                      [:scope, [:cvdecl, :@@blah, [:lit, 1]]]],
  },

  "dasgn_0"  => {
    "Ruby"        => "a.each { |x| b.each { |y| x = (x + 1) } if true }",
    "ParseTree"   => [:iter,
                      [:call, [:vcall, :a], :each],
                      [:dasgn_curr, :x],
                      [:if, [:true],
                       [:iter,
                        [:call, [:vcall, :b], :each],
                        [:dasgn_curr, :y],
                        [:dasgn, :x,
                         [:call, [:dvar, :x], :+, [:array, [:lit, 1]]]]],
                       nil]],
  },

  "dasgn_1"  => { # without mystery block / dasgn_curr
    "Ruby"        => "a.each { |x| b.each { |y| c = (c + 1) } if true }",
    "ParseTree"   => [:iter,
                      [:call, [:vcall, :a], :each],
                      [:dasgn_curr, :x],
                      [:if, [:true],
                       [:iter,
                        [:call, [:vcall, :b], :each],
                        [:dasgn_curr, :y],
                        [:dasgn_curr, :c,
                         [:call, [:dvar, :c], :+, [:array, [:lit, 1]]]]],
                       nil]],
  },

  "dasgn_2"  => { # WITH mystery block / dasgn_curr
    "Ruby"        => "a.each do |x|\n  if true then\n    c = 0\n    b.each { |y| c = (c + 1) }\n  \n  end\nend", # FIX: hate that extra newline!
    "ParseTree"   => [:iter,
                      [:call, [:vcall, :a], :each],
                      [:dasgn_curr, :x],
                      [:block,
                       [:dasgn_curr, :c],
                       [:if, [:true],
                        [:block,
                         [:dasgn_curr, :c, [:lit, 0]],
                         [:iter,
                          [:call, [:vcall, :b], :each],
                          [:dasgn_curr, :y],
                          [:dasgn, :c,
                           [:call, [:dvar, :c], :+, [:array, [:lit, 1]]]]]],
                        nil]]],
  },

  "dasgn_curr" => {
    "Ruby"        => "data.each do |x, y|\n  a = 1\n  b = a\n  b = a = x\nend",
    "ParseTree"   => [:iter,
                      [:call, [:vcall, :data], :each],
                      [:masgn, [:array, [:dasgn_curr, :x], [:dasgn_curr, :y]]],
                      [:block,
                       [:dasgn_curr, :a, [:lit, 1]],
                       [:dasgn_curr, :b, [:dvar, :a]],
                       [:dasgn_curr, :b, [:dasgn_curr, :a, [:dvar, :x]]]]],
  },

  "dasgn_icky"  => { # WITH mystery block / dasgn_curr
    "Ruby"        => "a do\n  v = nil\n  assert_block(full_message) do\n    begin\n      yield\n    rescue Exception => v\n      break\n    end\n  end\nend",
    "ParseTree"   => [:iter,
                      [:fcall, :a],
                      nil,
                      [:block,
                       [:dasgn_curr, :v],
                       [:dasgn_curr, :v, [:nil]],
                       [:iter,
                        [:fcall, :assert_block,
                         [:array, [:vcall, :full_message]]],
                        nil,
                        [:begin,
                         [:rescue,
                          [:yield],
                          [:resbody,
                           [:array, [:const, :Exception]],
                           [:block, [:dasgn, :v, [:gvar, :$!]], [:break]]]]]]]],
  },

  "dasgn_mixed" => {
    "Ruby"        => "t = 0\nns.each { |n| t += n }\n",
    "ParseTree"   => [:block,
                      [:lasgn, :t, [:lit, 0]],
                      [:iter,
                       [:call, [:vcall, :ns], :each],
                       [:dasgn_curr, :n],
                       [:lasgn, :t,
                        [:call, [:lvar, :t], :+, [:array, [:dvar, :n]]]]]],
    "Ruby2Ruby"   => "t = 0\nns.each { |n| t = (t + n) }\n",
  },

  "defined"  => {
    "Ruby"        => "defined? $x",
    "ParseTree"   => [:defined, [:gvar, :$x]],
  },

  "defn_args_mand_opt_block" => {
    "Ruby"      => "def x(a, b = 42, &d)\n  p(a, b, d)\nend",
    "ParseTree" => [:defn, :x,
                    [:scope,
                     [:block,
                      [:args, :a, :b,
                       [:block, [:lasgn, :b, [:lit, 42]]]],
                      [:block_arg, :d],
                      [:fcall, :p,
                       [:array, [:lvar, :a], [:lvar, :b], [:lvar, :d]]]]]]
  },

  "defn_args_mand_opt_splat" => {
    "Ruby"      => "def x(a, b = 42, *c)\n  p(a, b, c)\nend",
    "ParseTree" => [:defn, :x,
                    [:scope,
                     [:block,
                      [:args, :a, :b, :"*c",
                       [:block, [:lasgn, :b, [:lit, 42]]]],
                      [:fcall, :p,
                       [:array, [:lvar, :a], [:lvar, :b], [:lvar, :c]]]]]]
  },

  "defn_args_mand_opt_splat_block" => {
    "Ruby"      => "def x(a, b = 42, \*c, &d)\n  p(a, b, c, d)\nend",
    "ParseTree" => [:defn, :x,
                    [:scope,
                     [:block,
                      [:args, :a, :b, :"*c",
                       [:block, [:lasgn, :b, [:lit, 42]]]],
                      [:block_arg, :d],
                      [:fcall, :p,
                       [:array,
                        [:lvar, :a], [:lvar, :b],
                        [:lvar, :c], [:lvar, :d]]]]]]
  },

  "defn_args_mand_opt_splat_no_name" => {
    "Ruby"      => "def x(a, b = 42, *)\n  p(a, b)\nend",
    "ParseTree" => [:defn, :x,
                    [:scope,
                     [:block,
                      [:args, :a, :b, :"*",
                       [:block, [:lasgn, :b, [:lit, 42]]]],
                      [:fcall, :p,
                       [:array, [:lvar, :a], [:lvar, :b]]]]]]
  },

  "defn_args_opt_block" => {
    "Ruby"      => "def x(b = 42, &d)\n  p(b, d)\nend",
    "ParseTree" => [:defn, :x,
                    [:scope,
                     [:block,
                      [:args, :b,
                       [:block, [:lasgn, :b, [:lit, 42]]]],
                      [:block_arg, :d],
                      [:fcall, :p,
                       [:array, [:lvar, :b], [:lvar, :d]]]]]]
  },

  "defn_args_opt_splat_no_name" => {
    "Ruby"      => "def x(b = 42, *)\n  p(b)\nend",
    "ParseTree" => [:defn, :x,
                    [:scope,
                     [:block,
                      [:args, :b, :"*",
                       [:block, [:lasgn, :b, [:lit, 42]]]],
                      [:fcall, :p,
                       [:array, [:lvar, :b]]]]]]
  },

  "defn_empty" => {
    "Ruby"        => "def empty\n  # do nothing\nend",
    "ParseTree"   => [:defn, :empty, [:scope, [:block, [:args], [:nil]]]],
  },

  "defn_empty_args" => {
    "Ruby"        => "def empty(*)\n  # do nothing\nend",
    "ParseTree"   => [:defn, :empty, [:scope, [:block, [:args, :*], [:nil]]]],
  },

  "defn_lvar_boundary"  => { # FIX: add do nothing comment to block
    "Ruby"        => "mes = 42\ndef instantiate_all\n  Thread.new do\n    begin\n    rescue RuntimeError => mes\n      puts(mes)\n    end\n  end\nend\n",
    "ParseTree"   => [:block,
                      [:lasgn, :mes, [:lit, 42]],
                      [:defn, :instantiate_all,
                       [:scope,
                        [:block,
                         [:args],
                         [:iter,
                          [:call, [:const, :Thread], :new],
                          nil,
                          [:begin,
                           [:rescue,
                            [:resbody,
                             [:array, [:const, :RuntimeError]],
                             [:block,
                              [:dasgn_curr, :mes, [:gvar, :$!]],
                              [:fcall, :puts, [:array, [:dvar, :mes]]]]]]]]]]]],
  },

  "defn_optargs" => {
    "Ruby"      => "def x(a, *args)\n  p(a, args)\nend",
    "ParseTree" => [:defn, :x,
                    [:scope,
                     [:block,
                      [:args, :a, :"*args"],
                      [:fcall, :p,
                       [:array, [:lvar, :a], [:lvar, :args]]]]]],
  },

  "defn_or" => {
    "Ruby"        => "def |(o)\n  # do nothing\nend",
    "ParseTree"   => [:defn, :|, [:scope, [:block, [:args, :o], [:nil]]]],
  },

  "defn_rescue" => {
    "Ruby" => "def eql?(resource)\n  (self.uuid == resource.uuid)\nrescue\n  false\nend",
    "ParseTree" => [:defn, :eql?,
                    [:scope,
                     [:block,
                      [:args, :resource],
                      [:rescue,
                       [:call,
                        [:call, [:self], :uuid],
                        :==,
                        [:array, [:call, [:lvar, :resource], :uuid]]],
                       [:resbody, nil, [:false]]]]]],
    "Ruby2Ruby" => "def eql?(resource)\n  (self.uuid == resource.uuid) rescue false\nend",
  },

  "defn_something_eh" => {
    "Ruby"        => "def something?\n  # do nothing\nend",
    "ParseTree"   => [:defn, :something?, [:scope, [:block, [:args], [:nil]]]],
  },

  "defn_splat_no_name" => {
    "Ruby"      => "def x(a, *)\n  p(a)\nend",
    "ParseTree" => [:defn, :x,
                    [:scope,
                     [:block,
                      [:args, :a, :"*"],
                      [:fcall, :p,
                       [:array, [:lvar, :a]]]]]],
  },

  "defn_zarray" => { # tests memory allocation for returns
    "Ruby"        => "def zarray\n  a = []\n  return a\nend",
    "ParseTree"   => [:defn, :zarray,
                      [:scope,
                       [:block, [:args],
                        [:lasgn, :a, [:zarray]], [:return, [:lvar, :a]]]]],
  },

  "defs" => {
    "Ruby"      => "def self.x(y)\n  (y + 1)\nend",
    "ParseTree" => [:defs, [:self], :x,
                    [:scope,
                     [:block,
                      [:args, :y],
                      [:call, [:lvar, :y], :+, [:array, [:lit, 1]]]]]],
  },

  "defs_args_mand_opt_splat_block" => {
    "Ruby"      => "def self.x(a, b = 42, \*c, &d)\n  (a + b)\nend",
    "ParseTree" => [:defs, [:self], :x,
                    [:scope,
                     [:block,
                      [:args, :a, :b, :"*c",
                       [:block, [:lasgn, :b, [:lit, 42]]]],
                      [:block_arg, :d],
                      [:call, [:lvar, :a], :+, [:array, [:lvar, :b]]]]]],
  },

  "defs_empty" => {
    "Ruby"        => "def self.empty\n  # do nothing\nend",
    "ParseTree"   => [:defs, [:self], :empty,
                      [:scope, [:args]]],
  },

  "defs_empty_args" => {
    "Ruby"        => "def self.empty(*)\n  # do nothing\nend",
    "ParseTree"   => [:defs, [:self], :empty,
                      [:scope, [:args, :*]]],
  },

#   "dmethod" => {
#     "Ruby"        => [Examples, :dmethod_added],
#     "ParseTree"   => [:defn, :dmethod_added,
#                       [:dmethod,
#                        :a_method,
#                        [:scope,
#                         [:block,
#                          [:args, :x],
#                          [:call, [:lvar, :x], :+, [:array, [:lit, 1]]]]]]],
#     "Ruby2Ruby" => "def dmethod_added(x)\n  (x + 1)\nend"
#   },

  "dot2"  => {
    "Ruby"        => "(a..b)",
    "ParseTree"   => [:dot2, [:vcall, :a], [:vcall, :b]],
  },

  "dot3"  => {
    "Ruby"        => "(a...b)",
    "ParseTree"   => [:dot3, [:vcall, :a], [:vcall, :b]],
  },

  "dregx"  => {
    "Ruby"        => "/x#\{(1 + 1)}y/",
    "ParseTree"   => [:dregx, "x",
                      [:evstr, [:call, [:lit, 1], :+, [:array, [:lit, 1]]]], [:str, "y"]],
  },

  "dregx_interp" => {
    "Ruby"        => "/#\{@rakefile}/",
    "ParseTree"   => [:dregx, '', [:evstr, [:ivar, :@rakefile]]],
  },

  "dregx_n" => {
    "Ruby"        => '/#{1}/n',
    "ParseTree"   => [:dregx, '', [:evstr, [:lit, 1]], 16], # TODO: use consts
    "Ruby2Ruby"   => "/#\{1}/", # HACK - need to support regexp flags
  },

  "dregx_once"  => {
    "Ruby"        => "/x#\{(1 + 1)}y/o",
    "ParseTree"   => [:dregx_once, "x",
                      [:evstr, [:call, [:lit, 1], :+, [:array, [:lit, 1]]]], [:str, "y"]],
  },

  "dregx_once_n_interp" => {
    "Ruby"        => "/#\{IAC}#\{SB}/no",
    "ParseTree"   => [:dregx_once, '', [:evstr, [:const, :IAC]], [:evstr, [:const, :SB]], 16],
    "Ruby2Ruby"   => "/#\{IAC}#\{SB}/o", # HACK
  },

  "dstr" => {
    "Ruby"        => "argl = 1\n\"x#\{argl}y\"\n",
    "ParseTree"   => [:block,
                      [:lasgn, :argl, [:lit, 1]],
                      [:dstr, "x", [:evstr, [:lvar, :argl]],
                       [:str, "y"]]],
  },

  "dstr_2" => {
    "Ruby"        => "argl = 1\n\"x#\{(\"%.2f\" % 3.14159)}y\"\n",
    "ParseTree"   =>   [:block,
                        [:lasgn, :argl, [:lit, 1]],
                        [:dstr,
                         "x",
                         [:evstr, [:call, [:str, "%.2f"], :%, [:array, [:lit, 3.14159]]]],
                         [:str, "y"]]],
  },

  "dstr_3" => {
    "Ruby"        => "max = 2\nargl = 1\n\"x#\{(\"%.#\{max}f\" % 3.14159)}y\"\n",
    "ParseTree"   =>   [:block,
                        [:lasgn, :max, [:lit, 2]],
                        [:lasgn, :argl, [:lit, 1]],
                        [:dstr,
                         "x",
                         [:evstr, [:call,
                          [:dstr, "%.", [:evstr, [:lvar, :max]], [:str, "f"]],
                          :%, [:array, [:lit, 3.14159]]]],
                         [:str, "y"]]],
  },

  "dstr_concat"  => {
    "Ruby"        => '"#{22}aa" "cd#{44}" "55" "#{66}"',
    "ParseTree"   => [:dstr,
                      "",
                      [:evstr, [:lit, 22]],
                      [:str, "aa"],
                      [:str, "cd"],
                      [:evstr, [:lit, 44]],
                      [:str, "55"],
                      [:evstr, [:lit, 66]]],
    "Ruby2Ruby"   => '"#{22}aacd#{44}55#{66}"',
  },

  "dstr_heredoc_expand" => {
    "Ruby"        => "<<EOM\n  blah\n#\{1 + 1}blah\nEOM\n",
    "ParseTree"   => [:dstr, "  blah\n",
                      [:evstr, [:call, [:lit, 1], :+, [:array, [:lit, 1]]]],
                      [:str, "blah\n"]],
    "Ruby2Ruby"   => "\"  blah\\n#\{(1 + 1)}blah\\n\"",
  },

  "dstr_heredoc_windoze_sucks" => {
    "Ruby"        => "<<-EOF\r\ndef test_#\{action}_valid_feed\r\n  EOF\r\n",
    "ParseTree"   => [:dstr,
                      'def test_',
                      [:evstr, [:vcall, :action]],
                      [:str, "_valid_feed\n"]],
    "Ruby2Ruby"   => "\"def test_#\{action}_valid_feed\\n\"",
  },

  "dstr_heredoc_yet_again"  => {
    "Ruby"        => "<<-EOF\ns1 '#\{RUBY_PLATFORM}' s2\n#\{__FILE__}\n        EOF\n",
    "ParseTree"   => [:dstr, "s1 '",
                      [:evstr, [:const, :RUBY_PLATFORM]],
                      [:str, "' s2\n"],
                      [:str, "(string)"],
                      [:str, "\n"]],
    "Ruby2Ruby"   => "\"s1 '#\{RUBY_PLATFORM}' s2\\n(string)\\n\""
  },

  "dstr_nest"  => {
    "Ruby"        => "%Q[before [#\{nest}] after]",
    "ParseTree"   => [:dstr, "before [",
                      [:evstr, [:vcall, :nest]], [:str, "] after"]],
    "Ruby2Ruby"   => "\"before [#\{nest}] after\"",
  },

  "dstr_str_lit_start" => {
    "Ruby"        => '"#{"blah"}#{__FILE__}:#{__LINE__}: warning: #{$!.message} (#{$!.class})"',
    "ParseTree"   => [:dstr,
                      "blah(string):",
                      [:evstr, [:lit, 1]],
                      [:str, ": warning: "],
                      [:evstr, [:call, [:gvar, :$!], :message]],
                      [:str, " ("],
                      [:evstr, [:call, [:gvar, :$!], :class]],
                      [:str, ")"]],
    "Ruby2Ruby"   => '"blah(string):#{1}: warning: #{$!.message} (#{$!.class})"',
  },

  "dstr_the_revenge"  => {
    "Ruby"        => '"before #{from} middle #{to} (#{__FILE__}:#{__LINE__})"',
    "ParseTree"   => [:dstr,
                      "before ",
                      [:evstr, [:vcall, :from]],
                      [:str, " middle "],
                      [:evstr, [:vcall, :to]],
                      [:str, " ("],
                      [:str, "(string)"],
                      [:str, ":"],
                      [:evstr, [:lit, 1]],
                      [:str, ")"]],
    "Ruby2Ruby"   => '"before #{from} middle #{to} ((string):#{1})"',
  },

  "dsym"  => {
    "Ruby"        => ":\"x#\{(1 + 1)}y\"",
    "ParseTree"   => [:dsym, "x",
                      [:evstr, [:call, [:lit, 1], :+, [:array, [:lit, 1]]]], [:str, "y"]],
  },

  "dxstr" => {
    "Ruby"        => "t = 5\n`touch #\{t}`\n",
    "ParseTree"   => [:block,
                      [:lasgn, :t, [:lit, 5]],
                      [:dxstr, 'touch ', [:evstr, [:lvar, :t]]]],
  },

  "ensure" => {
    "Ruby"        => "begin\n  (1 + 1)\nrescue SyntaxError => e1\n  2\nrescue Exception => e2\n  3\nelse\n  4\nensure\n  5
end",
    "ParseTree"   => [:begin,
                      [:ensure,
                       [:rescue,
                        [:call, [:lit, 1], :+, [:array, [:lit, 1]]],
                        [:resbody,
                         [:array, [:const, :SyntaxError]],
                         [:block, [:lasgn, :e1, [:gvar, :$!]], [:lit, 2]],
                         [:resbody,
                          [:array, [:const, :Exception]],
                          [:block, [:lasgn, :e2, [:gvar, :$!]], [:lit, 3]]]],
                        [:lit, 4]],
                       [:lit, 5]]],
  },

  "false" => {
    "Ruby"      => "false",
    "ParseTree" => [:false],
  },

#   "fbody" => {
#     "Ruby"      => [Examples, :an_alias],
#     "ParseTree" => [:defn, :an_alias,
#                     [:fbody,
#                      [:scope,
#                       [:block,
#                        [:args, :x],
#                        [:call, [:lvar, :x], :+, [:array, [:lit, 1]]]]]]],
#     "Ruby2Ruby" => "def an_alias(x)\n  (x + 1)\nend"
#   },

  "fcall_arglist"  => {
    "Ruby"        => "m(42)",
    "ParseTree"   => [:fcall, :m, [:array, [:lit, 42]]],
  },

  "fcall_arglist_hash"  => {
    "Ruby"        => "m(:a => 1, :b => 2)",
    "ParseTree"   => [:fcall, :m,
                      [:array,
                       [:hash, [:lit, :a], [:lit, 1], [:lit, :b], [:lit, 2]]]],
  },

  "fcall_arglist_norm_hash"  => {
    "Ruby"        => "m(42, :a => 1, :b => 2)",
    "ParseTree"   => [:fcall, :m,
                      [:array,
                       [:lit, 42],
                       [:hash, [:lit, :a], [:lit, 1], [:lit, :b], [:lit, 2]]]],
  },

  "fcall_arglist_norm_hash_splat"  => {
    "Ruby"        => "m(42, :a => 1, :b => 2, *c)",
    "ParseTree"   => [:fcall, :m,
                      [:argscat,
                       [:array,
                        [:lit, 42],
                        [:hash, [:lit, :a], [:lit, 1], [:lit, :b], [:lit, 2]]],
                       [:vcall, :c]]],
  },

  "fcall_block"  => {
    "Ruby"        => "a(:b) { :c }",
    "ParseTree"   => [:iter,
                      [:fcall, :a, [:array, [:lit, :b]]], nil,
                      [:lit, :c]],
  },

  "fcall_keyword"  => {
    "Ruby"        => "42 if block_given?",
    "ParseTree"   => [:if, [:fcall, :block_given?], [:lit, 42], nil],
  },

  "flip2"  => {
    "Ruby"        => "x = if ((i % 4) == 0)..((i % 3) == 0) then\n  i\nelse\n  nil\nend",
    "ParseTree"   => [:lasgn,
                      :x,
                      [:if,
                       [:flip2,
                        [:call,
                         [:call, [:vcall, :i], :%, [:array, [:lit, 4]]],
                         :==,
                         [:array, [:lit, 0]]],
                        [:call,
                         [:call, [:vcall, :i], :%, [:array, [:lit, 3]]],
                         :==,
                         [:array, [:lit, 0]]]],
                       [:vcall, :i],
                       [:nil]]],
  },

  "flip2_method"  => {
    "Ruby"        => "if 1..2.a?(b) then
  nil
end",
    "ParseTree"   => [:if,
                      [:flip2,
                       [:lit, 1],
                       [:call, [:lit, 2], :a?, [:array, [:vcall, :b]]]],
                      [:nil],
                      nil],
  },

  "flip3"  => {
    "Ruby"        => "x = if ((i % 4) == 0)...((i % 3) == 0) then\n  i\nelse\n  nil\nend",
    "ParseTree"   => [:lasgn,
                      :x,
                      [:if,
                       [:flip3,
                        [:call,
                         [:call, [:vcall, :i], :%, [:array, [:lit, 4]]],
                         :==,
                         [:array, [:lit, 0]]],
                        [:call,
                         [:call, [:vcall, :i], :%, [:array, [:lit, 3]]],
                         :==,
                         [:array, [:lit, 0]]]],
                       [:vcall, :i],
                       [:nil]]],
  },

  "for"  => {
    "Ruby"        => "for o in ary do\n  puts(o)\nend",
    "ParseTree"   => [:for,
                      [:vcall, :ary],
                      [:lasgn, :o],
                      [:fcall, :puts, [:array, [:lvar, :o]]]],
  },

  "for_no_body"  => {
    "Ruby"        => "for i in (0..max) do\n  # do nothing\nend",
    "ParseTree"   => [:for,
                      [:dot2, [:lit, 0], [:vcall, :max]],
                      [:lasgn, :i]],
  },

  "gasgn"  => {
    "Ruby"        => "$x = 42",
    "ParseTree"   => [:gasgn, :$x, [:lit, 42]],
  },

  "global" => {
    "Ruby"        => "$stderr",
    "ParseTree"   =>  [:gvar, :$stderr],
  },

  "gvar"  => {
    "Ruby"        => "$x",
    "ParseTree"   => [:gvar, :$x],
  },

  "gvar_underscore"  => {
    "Ruby"        => "$_",
    "ParseTree"   => [:gvar, :$_],
  },

  "gvar_underscore_blah"  => {
    "Ruby"        => "$__blah",
    "ParseTree"   => [:gvar, :$__blah],
  },

  "hash"  => {
    "Ruby"        => "{ 1 => 2, 3 => 4 }",
    "ParseTree"   => [:hash, [:lit, 1], [:lit, 2], [:lit, 3], [:lit, 4]],
  },

  "hash_rescue"  => {
    "Ruby"        => "{ 1 => (2 rescue 3) }",
    "ParseTree"   => [:hash,
                      [:lit, 1],
                      [:rescue, [:lit, 2], [:resbody, nil, [:lit, 3]]]],
  },

  "iasgn"  => {
    "Ruby"        => "@a = 4",
    "ParseTree"   => [:iasgn, :@a, [:lit, 4]],
  },

  "if_block_condition" => {
    "Ruby"        => "if (x = 5\n(x + 1)) then\n  nil\nend",
    "ParseTree"   => [:if,
                      [:block,
                       [:lasgn, :x, [:lit, 5]],
                       [:call,
                        [:lvar, :x],
                        :+,
                        [:array, [:lit, 1]]]],
                      [:nil],
                      nil],
  },

  "if_lasgn_short" => {
    "Ruby"        => "if x = obj.x then\n  x.do_it\nend",
    "ParseTree"   => [:if,
                      [:lasgn, :x,
                       [:call, [:vcall, :obj], :x]],
                      [:call,
                       [:lvar, :x], :do_it],
                      nil],
  },

  "iteration1" => {
    "Ruby"        => "loop { }",
    "ParseTree"   => [:iter, [:fcall, :loop], nil],
  },

  "iteration2" => {
    "Ruby" => "array = [1, 2, 3]\narray.each { |x| puts(x.to_s) }\n",
    "ParseTree"   => [:block,
                      [:lasgn, :array,
                       [:array, [:lit, 1], [:lit, 2], [:lit, 3]]],
                      [:iter,
                       [:call, [:lvar, :array], :each],
                       [:dasgn_curr, :x],
                       [:fcall, :puts, [:array, [:call, [:dvar, :x], :to_s]]]]],
  },

  "iteration3" => {
    "Ruby"        => "1.upto(3) { |n| puts(n.to_s) }",
    "ParseTree"   => [:iter,
                      [:call, [:lit, 1], :upto, [:array, [:lit, 3]]],
                      [:dasgn_curr, :n],
                      [:fcall, :puts, [:array, [:call, [:dvar, :n], :to_s]]]],
  },

  "iteration4" => {
    "Ruby"        => "3.downto(1) { |n| puts(n.to_s) }",
    "ParseTree"   => [:iter,
                      [:call, [:lit, 3], :downto, [:array, [:lit, 1]]],
                      [:dasgn_curr, :n],
                      [:fcall, :puts, [:array, [:call, [:dvar, :n], :to_s]]]],
  },

  "iteration5" => {
    "Ruby"        => "argl = 10\nwhile (argl >= 1) do\n  puts(\"hello\")\n  argl = (argl - 1)\nend\n",
    "ParseTree"   => [:block,
                      [:lasgn, :argl, [:lit, 10]],
                      [:while,
                       [:call, [:lvar, :argl], :">=", [:array, [:lit, 1]]],
                       [:block,
                        [:fcall, :puts, [:array, [:str, "hello"]]],
                        [:lasgn,
                         :argl,
                         [:call, [:lvar, :argl],
                          :"-", [:array, [:lit, 1]]]]], true]],
  },

  "iteration6" => {
    "Ruby"      => "array1 = [1, 2, 3]\narray2 = [4, 5, 6, 7]\narray1.each do |x|\n  array2.each do |y|\n    puts(x.to_s)\n    puts(y.to_s)\n  end\nend\n",
    "ParseTree" => [:block,
                    [:lasgn, :array1,
                     [:array, [:lit, 1], [:lit, 2], [:lit, 3]]],
                    [:lasgn, :array2,
                     [:array, [:lit, 4], [:lit, 5], [:lit, 6], [:lit, 7]]],
                    [:iter,
                     [:call,
                      [:lvar, :array1], :each],
                     [:dasgn_curr, :x],
                     [:iter,
                      [:call,
                       [:lvar, :array2], :each],
                      [:dasgn_curr, :y],
                      [:block,
                       [:fcall, :puts,
                        [:array, [:call, [:dvar, :x], :to_s]]],
                       [:fcall, :puts,
                        [:array, [:call, [:dvar, :y], :to_s]]]]]]],
  },

  "iteration7" => {
    "Ruby"        => "a { |b, c| p(c) }",
    "ParseTree"   => [:iter,
                      [:fcall, :a],
                      [:masgn,
                       [:array, [:dasgn_curr, :b], [:dasgn_curr, :c]]],
                      [:fcall, :p, [:array, [:dvar, :c]]]],
  },

  "iteration8" => {
    "Ruby"        => "a { |b, c, *d| p(c) }",
    "ParseTree"   => [:iter,
                      [:fcall, :a],
                      [:masgn,
                       [:array, [:dasgn_curr, :b], [:dasgn_curr, :c]],
                       [:dasgn_curr, :d]],
                      [:fcall, :p, [:array, [:dvar, :c]]]],
  },

  "iteration9" => {
    "Ruby"        => "a { |b, c, *| p(c) }",
    "ParseTree"   => [:iter,
                      [:fcall, :a],
                      [:masgn,
                       [:array, [:dasgn_curr, :b], [:dasgn_curr, :c]],
                       [:splat]],
                      [:fcall, :p, [:array, [:dvar, :c]]]],
  },

  "iteration9" => {
    "Ruby"        => "a { |*| p(c) }",
    "ParseTree"   => [:iter,
                      [:fcall, :a],
                      [:masgn,
                       [:splat]],
                      [:fcall, :p, [:array, [:vcall, :c]]]],
  },

  "iteration_dasgn_curr_dasgn_madness"  => {
    "Ruby"        => "as.each { |a|\n  b += a.b(false) }",
    "ParseTree"   => [:iter,
                      [:call, [:vcall, :as], :each],
                      [:dasgn_curr, :a],
                      [:dasgn_curr,
                       :b,
                       [:call,
                        [:dvar, :b],
                        :+,
                        [:array,
                         [:call, [:dvar, :a], :b, [:array, [:false]]]]]]],
    "Ruby2Ruby"   => "as.each { |a| b = (b + a.b(false)) }",
  },

  "iteration_double_var" => {
    "Ruby"        => "a do |x|\n  b do |x| \n    puts x\n  end\nend",
    "ParseTree"   => [:iter,
                      [:fcall, :a],
                      [:dasgn_curr, :x],
                      [:iter,
                       [:fcall, :b],
                       [:dasgn, :x],
                       [:fcall, :puts, [:array, [:dvar, :x]]]]],
    "Ruby2Ruby"   => "a { |x| b { |x| puts(x) } }",
  },

  "iteration_masgn"  => {
    "Ruby"        => "define_method(method) { |*args| }",
    "ParseTree"   => [:iter,
                      [:fcall, :define_method, [:array, [:vcall, :method]]],
                      [:masgn, [:dasgn_curr, :args]]],
  },

#   "ivar" => {
#     "Ruby"        => [Examples, :reader],
#     "ParseTree"   => [:defn, :reader, [:ivar, :@reader]],
#     "Ruby2Ruby"   => "attr_reader :reader"
#   },

  "lasgn_array" => {
    "Ruby"        => "var = [\"foo\", \"bar\"]",
    "ParseTree"   => [:lasgn, :var,
                      [:array,
                       [:str, "foo"],
                       [:str, "bar"]]],
  },

  "lasgn_call" => {
    "Ruby"        => "c = (2 + 3)",
    "ParseTree"   => [:lasgn, :c, [:call, [:lit, 2], :+, [:array, [:lit, 3]]]],
  },

  "lit_bool_false" => {
    "Ruby"        => "false",
    "ParseTree"   => [:false],
  },

  "lit_bool_true" => {
    "Ruby"        => "true",
    "ParseTree"   => [:true],
  },

  "lit_float" => {
    "Ruby"        => "1.1",
    "ParseTree"   => [:lit, 1.1],
  },

  "lit_long" => {
    "Ruby"        => "1",
    "ParseTree"   => [:lit, 1],
  },

  "lit_long_negative" => {
    "Ruby"        => "-1",
    "ParseTree"   => [:lit, -1],
  },

  "lit_range2" => {
    "Ruby"        => "(1..10)",
    "ParseTree"   => [:lit, 1..10],
  },

  "lit_range3" => {
    "Ruby"        => "(1...10)",
    "ParseTree"   => [:lit, 1...10],
  },

  "lit_regexp" => {
    "Ruby"        => "/x/",
    "ParseTree"   => [:lit, /x/],
  },

  "lit_regexp_i_wwtt"  => {
    "Ruby"        => 'str.split(//i)',
    "ParseTree"   => [:call, [:vcall, :str], :split, [:array, [:lit, //i]]],
  },

  "lit_regexp_n" => {
    "Ruby"        => "/x/n",
    "ParseTree"   => [:lit, /x/n],
  },

  "lit_regexp_once" => {
    "Ruby"        => "/x/o",
    "ParseTree"   => [:lit, /x/],
    "Ruby2Ruby"   => "/x/",
  },

  "lit_sym" => {
    "Ruby"        => ":x",
    "ParseTree"   => [:lit, :x],
  },

  "lit_sym_splat" => {
    "Ruby"        => ":\"*args\"",
    "ParseTree"   => [:lit, :"*args"],
  },

  "lvar_def_boundary" => { # HACK: put # do nothing back under begin
    "Ruby"        => "b = 42\ndef a\n  c do\n    begin\n    rescue RuntimeError => b\n      puts(b)\n    end\n  end\nend\n",
    "ParseTree"   => [:block,
                      [:lasgn, :b, [:lit, 42]],
                      [:defn, :a,
                       [:scope,
                        [:block,
                         [:args],
                         [:iter,
                          [:fcall, :c],
                          nil,
                          [:begin,
                           [:rescue,
                            [:resbody,
                             [:array, [:const, :RuntimeError]],
                             [:block,
                              [:dasgn_curr, :b, [:gvar, :$!]],
                              [:fcall, :puts, [:array, [:dvar, :b]]]]]]]]]]]],
  },

  "masgn"  => {
    "Ruby"        => "a, b = c, d",
    "ParseTree"   => [:masgn,
                      [:array, [:lasgn, :a], [:lasgn, :b]],
                      [:array,  [:vcall, :c], [:vcall, :d]]],
  },

  "masgn_argscat"  => {
    "Ruby"        => "a, b, *c = 1, 2, *[3, 4]",
    "ParseTree"   => [:masgn,
                      [:array, [:lasgn, :a], [:lasgn, :b]],
                      [:lasgn, :c],
                      [:argscat,
                       [:array, [:lit, 1], [:lit, 2]],
                       [:array, [:lit, 3], [:lit, 4]]]],
  },

  "masgn_attrasgn"  => {
    "Ruby"        => "a, b.c = d, e",
    "ParseTree"   => [:masgn,
                       [:array, [:lasgn, :a], [:attrasgn, [:vcall, :b], :c=]],
                       [:array, [:vcall, :d], [:vcall, :e]]],
  },

  "masgn_iasgn"  => {
    "Ruby"        => "a, @b = c, d",
    "ParseTree"   => [:masgn,
                      [:array, [:lasgn, :a], [:iasgn, :"@b"]],
                      [:array,  [:vcall, :c], [:vcall, :d]]],
  },

  "masgn_masgn" => {
    "Ruby"        => "a, (b, c) = [1, [2, 3]]",
    "ParseTree"   => [:masgn,
                      [:array,
                       [:lasgn, :a],
                       [:masgn,
                        [:array,
                         [:lasgn, :b],
                         [:lasgn, :c]]]],
                      [:to_ary,
                       [:array,
                        [:lit, 1],
                        [:array,
                         [:lit, 2],
                         [:lit, 3]]]]]

  },

  "masgn_splat"  => {
    "Ruby"        => "a, b, *c = d, e, f, g",
    "ParseTree"   => [:masgn,
                      [:array, [:lasgn, :a], [:lasgn, :b]],
                      [:lasgn, :c],
                      [:array,
                       [:vcall, :d], [:vcall, :e],
                       [:vcall, :f], [:vcall, :g]]]
  },

  "masgn_splat_no_name_to_ary"  => {
    "Ruby"        => "a, b, * = c",
    "ParseTree"   => [:masgn,
                      [:array, [:lasgn, :a], [:lasgn, :b]],
                      [:splat],
                      [:to_ary, [:vcall, :c]]],
  },

  "masgn_splat_no_name_trailing"  => {
    "Ruby"        => "a, b, = c",
    "ParseTree"   => [:masgn,
                      [:array, [:lasgn, :a], [:lasgn, :b]],
                      [:to_ary, [:vcall, :c]]],
    "Ruby2Ruby"   => "a, b = c", # TODO: check this is right
  },

  "masgn_splat_to_ary"  => {
    "Ruby"        => "a, b, *c = d",
    "ParseTree"   => [:masgn,
                       [:array, [:lasgn, :a], [:lasgn, :b]],
                       [:lasgn, :c],
                       [:to_ary, [:vcall, :d]]],
  },

  "masgn_splat_to_ary2"  => {
    "Ruby"        => "a, b, *c = d.e(\"f\")",
    "ParseTree"   => [:masgn,
                      [:array, [:lasgn, :a], [:lasgn, :b]],
                      [:lasgn, :c],
                      [:to_ary,
                       [:call, [:vcall, :d], :e, [:array, [:str, 'f']]]]],
  },

  "match"  => {
    "Ruby"        => "1 if /x/",
    "ParseTree"   => [:if, [:match, [:lit, /x/]], [:lit, 1], nil],
  },

  "match2" => {
    "Ruby"        => "/x/ =~ \"blah\"",
    "ParseTree"   => [:match2, [:lit, /x/], [:str, "blah"]],
  },

  "match3" => {
    "Ruby"        => "\"blah\" =~ /x/",
    "ParseTree"   => [:match3, [:lit, /x/], [:str, "blah"]],
  },

  "module"  => {
    "Ruby"        => "module X\n  def y\n    # do nothing\n  end\nend",
    "ParseTree"   => [:module, :X,
                      [:scope,
                       [:defn, :y, [:scope, [:block, [:args], [:nil]]]]]],
  },

  "next"  => {
    "Ruby"        => "loop { next if false }",
    "ParseTree"   => [:iter,
                      [:fcall, :loop],
                      nil,
                      [:if, [:false], [:next], nil]],
  },

  "next_arg"  => {
    "Ruby"        => "loop { next 42 if false }",
    "ParseTree"   => [:iter,
                      [:fcall, :loop],
                      nil,
                      [:if, [:false], [:next, [:lit, 42]], nil]],
  },

  "not"  => {
    "Ruby"        => "(not true)",
    "ParseTree"   => [:not, [:true]],
  },

  "nth_ref"  => {
    "Ruby"        => "$1",
    "ParseTree"   => [:nth_ref, 1],
  },

  "op_asgn1" => {
    "Ruby"        => "b = []\nb[1] ||= 10\nb[2] &&= 11\nb[3] += 12\n",
    "ParseTree"   => [:block,
                      [:lasgn, :b, [:zarray]],
                      [:op_asgn1, [:lvar, :b],
                       [:array, [:lit, 1]], :"||", [:lit, 10]],
                      [:op_asgn1, [:lvar, :b],
                       [:array, [:lit, 2]], :"&&", [:lit, 11]],
                      [:op_asgn1, [:lvar, :b],
                       [:array, [:lit, 3]], :+, [:lit, 12]]],
  },

  "op_asgn2" => {
    "Ruby"        => "s = Struct.new(:var)\nc = s.new(nil)\nc.var ||= 20\nc.var &&= 21\nc.var += 22\nc.d.e.f ||= 42\n",
    "ParseTree"   => [:block,
                      [:lasgn, :s,
                       [:call, [:const, :Struct],
                        :new, [:array, [:lit, :var]]]],
                      [:lasgn, :c,
                       [:call, [:lvar, :s], :new, [:array, [:nil]]]],

                      [:op_asgn2, [:lvar, :c], :var=, :"||",
                       [:lit, 20]],
                      [:op_asgn2, [:lvar, :c], :var=, :"&&",
                       [:lit, 21]],
                      [:op_asgn2, [:lvar, :c], :var=, :+, [:lit, 22]],

                      [:op_asgn2,
                       [:call,
                        [:call, [:lvar, :c], :d], :e], :f=, :"||",
                       [:lit, 42]]],
  },

  "op_asgn2_self"  => {
    "Ruby"        => "self.Bag ||= Bag.new",
    "ParseTree"   => [:op_asgn2, [:self], :"Bag=", :"||", [:call, [:const, :Bag], :new]],
  },

  "op_asgn_and" => {
    "Ruby"        => "a = 0\na &&= 2\n",
    "ParseTree"   => [:block,
                      [:lasgn, :a, [:lit, 0]],
                      [:op_asgn_and, [:lvar, :a], [:lasgn, :a, [:lit, 2]]]],
  },

  "op_asgn_and_ivar2" => { # eww... stupid rubygems
    "Ruby"        => "@fetcher &&= new(Gem.configuration[:http_proxy])",
    "ParseTree"   => [:op_asgn_and,
                      [:ivar, :@fetcher],
                      [:iasgn,
                       :@fetcher,
                       [:fcall,
                        :new,
                        [:array,
                         [:call,
                          [:call, [:const, :Gem], :configuration],
                          :[],
                          [:array, [:lit, :http_proxy]]]]]]],
  },

  "op_asgn_or" => {
    "Ruby"        => "a = 0\na ||= 1\n",
    "ParseTree"   => [:block,
                      [:lasgn, :a, [:lit, 0]],
                      [:op_asgn_or, [:lvar, :a], [:lasgn, :a, [:lit, 1]]]],
  },

  "op_asgn_or_block"  => {
    "Ruby"        => "a ||= begin\n        b\n      rescue\n        c\n      end",
    "ParseTree"   => [:op_asgn_or,
                      [:lvar, :a],
                      [:lasgn, :a,
                       [:rescue,
                        [:vcall, :b],
                        [:resbody, nil, [:vcall, :c]]]]],
    "Ruby2Ruby"   => "a ||= b rescue c",
  },

  "op_asgn_or_ivar" => {
    "Ruby"        => "@v ||= {  }",
    "ParseTree"   => [:op_asgn_or, [:ivar, :@v], [:iasgn, :@v, [:hash]]],
  },

  "op_asgn_or_ivar2" => { # eww... stupid rubygems
    "Ruby"        => "@fetcher ||= new(Gem.configuration[:http_proxy])",
    "ParseTree"   => [:op_asgn_or,
                      [:ivar, :@fetcher],
                      [:iasgn,
                       :@fetcher,
                       [:fcall,
                        :new,
                        [:array,
                         [:call,
                          [:call, [:const, :Gem], :configuration],
                          :[],
                          [:array, [:lit, :http_proxy]]]]]]],
  },

  "or"  => {
    "Ruby"        => "(a or b)",
    "ParseTree"   => [:or, [:vcall, :a], [:vcall, :b]],
  },

  "or_big"  => {
    "Ruby"        => "((a or b) or (c and d))",
    "ParseTree"   => [:or,
                      [:or,  [:vcall, :a], [:vcall, :b]],
                      [:and, [:vcall, :c], [:vcall, :d]]],
  },

  "or_big2"  => {
    "Ruby"        => "((a || b) || (c && d))",
    "ParseTree"   => [:or,
                      [:or,  [:vcall, :a], [:vcall, :b]],
                      [:and, [:vcall, :c], [:vcall, :d]]],
    "Ruby2Ruby"   => "((a or b) or (c and d))",
  },

  "postexe"  => {
    "Ruby"        => "END { 1 }",
    "ParseTree"   => [:iter, [:postexe], nil, [:lit, 1]],
  },

  "proc_args_0" => {
    "Ruby" => "proc { || (x + 1) }",
    "ParseTree" => [:iter,
                    [:fcall, :proc],
                    0,
                    [:call, [:vcall, :x], :+, [:array, [:lit, 1]]]],
  },

  "proc_args_1" => {
    "Ruby" => "proc { |x| (x + 1) }",
    "ParseTree" => [:iter,
                    [:fcall, :proc],
                    [:dasgn_curr, :x],
                    [:call, [:dvar, :x], :+, [:array, [:lit, 1]]]],
  },

  "proc_args_2" => {
    "Ruby" => "proc { |x, y| (x + y) }",
    "ParseTree" => [:iter,
                    [:fcall, :proc],
                    [:masgn, [:array, [:dasgn_curr, :x], [:dasgn_curr, :y]]],
                    [:call, [:dvar, :x], :+, [:array, [:dvar, :y]]]],
  },

  "proc_args_no" => {
    "Ruby" => "proc { (x + 1) }",
    "ParseTree" => [:iter,
                    [:fcall, :proc],
                    nil,
                    [:call, [:vcall, :x], :+, [:array, [:lit, 1]]]],
  },

  "redo"  => {
    "Ruby"        => "loop { redo if false }",
    "ParseTree"   => [:iter,
                      [:fcall, :loop], nil, [:if, [:false], [:redo], nil]],
  },

  "rescue"  => {
    "Ruby"        => "blah rescue nil",
    "ParseTree"   => [:rescue, [:vcall, :blah], [:resbody, nil, [:nil]]],
  },

  "rescue_block_body"  => {
    "Ruby"        => "begin\n  a\nrescue => e\n  c\n  d\nend",
    "ParseTree"   => [:begin,
                      [:rescue,
                       [:vcall, :a],
                       [:resbody, nil,
                        [:block,
                         [:lasgn, :e, [:gvar, :$!]],
                         [:vcall, :c],
                         [:vcall, :d]]]]],
  },

  "rescue_block_nada"  => {
    "Ruby"        => "begin\n  blah\nrescue\n  # do nothing\nend",
    "ParseTree"   => [:begin, [:rescue, [:vcall, :blah], [:resbody, nil]]]
  },

  "rescue_exceptions"  => {
    "Ruby"        => "begin\n  blah\nrescue RuntimeError => r\n  # do nothing\nend",
    "ParseTree"   => [:begin,
                      [:rescue,
                       [:vcall, :blah],
                       [:resbody,
                        [:array, [:const, :RuntimeError]],
                        [:lasgn, :r, [:gvar, :$!]]]]],
  },

  "retry"  => {
    "Ruby"        => "retry",
    "ParseTree"   => [:retry],
  },

  "return_0"  => {
    "Ruby"        => "return",
    "ParseTree"   => [:return],
  },

  "return_1"  => {
    "Ruby"        => "return 1",
    "ParseTree"   => [:return, [:lit, 1]],
  },

  "return_n"  => {
    "Ruby"        => "return 1, 2, 3",
    "ParseTree"   => [:return, [:array, [:lit, 1], [:lit, 2], [:lit, 3]]],
    "Ruby2Ruby"   => "return [1, 2, 3]",
  },

  "sclass"  => {
    "Ruby"        => "class << self\n  42\nend",
    "ParseTree"   => [:sclass, [:self], [:scope, [:lit, 42]]],
  },

  "sclass_trailing_class" => {
    "Ruby" => "class A\n  class << self\n    a\n  end\n  class B\n  end\nend",
    "ParseTree" => [:class, :A, nil,
                    [:scope,
                     [:block,
                      [:sclass, [:self], [:scope, [:vcall, :a]]],
                      [:class, :B, nil, [:scope]]]]],
  },

  "splat"  => {
    "Ruby"        => "def x(*b)\n  a(*b)\nend",
    "ParseTree"   => [:defn, :x,
                      [:scope,
                       [:block,
                        [:args, :"*b"],
                        [:fcall, :a, [:splat, [:lvar, :b]]]]]],
  },

  "str" => {
    "Ruby"        => '"x"',
    "ParseTree"   => [:str, "x"],
  },

  "str_concat_newline" => {
    "Ruby"        => '"before" \\
  " after"',
    "ParseTree"   => [:str, "before after"],
    "Ruby2Ruby"   => '"before after"',
  },

  "str_concat_space" => {
    "Ruby"        => '"before" " after"',
    "ParseTree"   => [:str, "before after"],
    "Ruby2Ruby"   => '"before after"',
  },

  "str_heredoc" => {
    "Ruby"        => "<<'EOM'\n  blah\nblah\nEOM\n",
    "ParseTree"   => [:str, "  blah\nblah\n"],
    "Ruby2Ruby"   => "\"  blah\\nblah\\n\"",
  },

  "str_heredoc_call" => {
    "Ruby"        => "<<'EOM'.strip\n  blah\nblah\nEOM\n",
    "ParseTree"   => [:call, [:str, "  blah\nblah\n"], :strip],
    "Ruby2Ruby"   => "\"  blah\\nblah\\n\".strip",
  },

  "str_heredoc_double" => {
    "Ruby"       => "a += <<-BEGIN + b + <<-END\n  first\nBEGIN\n  second\nEND",
    "ParseTree"  => [:lasgn, :a,
                     [:call,
                      [:lvar, :a],
                      :+,
                      [:array,
                       [:call,
                        [:call, [:str, "  first\n"], :+,
                         [:array, [:vcall, :b]]],
                        :+,
                        [:array, [:str, "  second\n"]]]]]],
    "Ruby2Ruby"   => "a = (a + ((\"  first\\n\" + b) + \"  second\\n\"))",
  },

  "str_heredoc_indent" => {
    "Ruby"        => "<<-EOM\n  blah\nblah\n\n  EOM\n",
    "ParseTree"   => [:str, "  blah\nblah\n\n"],
    "Ruby2Ruby"   => "\"  blah\\nblah\\n\\n\"",
  },

  "str_interp_file" => {
    "Ruby"        => '"file = #{__FILE__}
"',
    "ParseTree"   => [:str, "file = (string)\n"],
    "Ruby2Ruby"   => '"file = (string)\\n"',
  },

  "structure_extra_block_for_dvar_scoping"  => {
    "Ruby"        => "a.b do |c, d|\n  unless e.f(c) then\n    g = false\n    d.h { |x, i| g = true }\n  \n  end\nend", # FIX: don't like the extra return
    "ParseTree"   => [:iter,
                      [:call, [:vcall, :a], :b],
                      [:masgn, [:array, [:dasgn_curr, :c], [:dasgn_curr, :d]]],
                      [:block,
                       [:dasgn_curr, :g],
                       [:if,
                        [:call, [:vcall, :e], :f, [:array, [:dvar, :c]]],
                        nil,
                        [:block,
                         [:dasgn_curr, :g, [:false]],
                         [:iter,
                          [:call, [:dvar, :d], :h],
                          [:masgn, [:array, [:dasgn_curr, :x], [:dasgn_curr, :i]]],
                          [:dasgn, :g, [:true]]]]]]],
    },

  "structure_remove_begin_1"  => {
    "Ruby"        => "a << begin\n       b\n     rescue\n       c\n     end",
    "ParseTree"   => [:call, [:vcall, :a], :<<,
                      [:array, [:rescue, [:vcall, :b],
                                [:resbody, nil, [:vcall, :c]]]]],
    "Ruby2Ruby"   => "(a << b rescue c)",
  },

  "structure_remove_begin_2"  => {
    "Ruby"        => "a = if c\n      begin\n        b\n      rescue\n        nil\n      end\n    end\na",
    "ParseTree"   => [:block,
                      [:lasgn,
                       :a,
                       [:if, [:vcall, :c],
                        [:rescue, [:vcall, :b], [:resbody, nil, [:nil]]],
                        nil]],
                      [:lvar, :a]],
    "Ruby2Ruby"   => "a = b rescue nil if c\na\n", # OMG that's awesome
  },

  "structure_unused_literal_wwtt"  => {
    "Ruby"        => "\"prevent the above from infecting rdoc\"\n\nmodule Graffle\nend",
    "ParseTree"   => [:module, :Graffle, [:scope]],
    "Ruby2Ruby"   => "module Graffle\nend",
  },

  # TODO: all supers need to pass args
  "super"  => {
    "Ruby"        => "def x\n  super(4)\nend",
    "ParseTree"   => [:defn, :x,
                      [:scope,
                       [:block,
                        [:args],
                        [:super, [:array, [:lit, 4]]]]]],
  },

  "super_block_pass"  => {
    "Ruby"        => "super(a, &b)",
    "ParseTree"   => [:block_pass,
                      [:vcall, :b], [:super, [:array, [:vcall, :a]]]],
  },

  "super_block_splat"  => {
    "Ruby"        => "super(a, *b)",
    "ParseTree"   => [:super,
                      [:argscat,
                       [:array, [:vcall, :a]],
                       [:vcall, :b]]],
  },

  "super_multi"  => {
    "Ruby"        => "def x\n  super(4, 2, 1)\nend",
    "ParseTree"   => [:defn, :x,
                      [:scope,
                       [:block,
                        [:args],
                        [:super, [:array, [:lit, 4], [:lit, 2], [:lit, 1]]]]]],
  },

  "svalue"  => {
    "Ruby"        => "a = *b",
    "ParseTree"   => [:lasgn, :a, [:svalue, [:splat, [:vcall, :b]]]],
  },

  "to_ary"  => {
    "Ruby"        => "a, b = c",
    "ParseTree"   => [:masgn,
                      [:array, [:lasgn, :a], [:lasgn, :b]],
                      [:to_ary, [:vcall, :c]]],
  },

  "true" => {
    "Ruby"      => "true",
    "ParseTree" => [:true],
  },

  "undef"  => {
    "Ruby"        => "undef :x",
    "ParseTree"   => [:undef, [:lit, :x]],
  },

  "undef_2"  => {
    "Ruby"        => "undef :x, :y",
    "ParseTree"   => [:block,
                      [:undef, [:lit, :x]],
                      [:undef, [:lit, :y]]],
    "Ruby2Ruby"   => "undef :x\nundef :y\n",
  },

  "undef_3"  => {
    "Ruby"        => "undef :x, :y, :z",
    "ParseTree"   => [:block,
                      [:undef, [:lit, :x]],
                      [:undef, [:lit, :y]],
                      [:undef, [:lit, :z]]],
    "Ruby2Ruby"   => "undef :x\nundef :y\nundef :z\n",
  },

  "undef_block_1"  => {
    "Ruby"        => "f1\nundef :x\n", # TODO: don't like the extra return
    "ParseTree"   => [:block,
                      [:vcall, :f1],
                      [:undef, [:lit, :x]]],
  },

  "undef_block_2"  => {
    "Ruby"        => "f1\nundef :x, :y",
    "ParseTree"   => [:block,
                      [:vcall, :f1],
                      [:block,
                       [:undef, [:lit, :x]],
                       [:undef, [:lit, :y]],
                      ]],
    "Ruby2Ruby"   => "f1\n(undef :x\nundef :y)\n",
  },

  "undef_block_3"  => {
    "Ruby"        => "f1\nundef :x, :y, :z",
    "ParseTree"   => [:block,
                      [:vcall, :f1],
                      [:block,
                       [:undef, [:lit, :x]],
                       [:undef, [:lit, :y]],
                       [:undef, [:lit, :z]],
                      ]],
    "Ruby2Ruby"   => "f1\n(undef :x\nundef :y\nundef :z)\n",
  },

  "undef_block_3_post"  => {
    "Ruby"        => "undef :x, :y, :z\nf2",
    "ParseTree"   => [:block,
                      [:undef, [:lit, :x]],
                      [:undef, [:lit, :y]],
                      [:undef, [:lit, :z]],
                      [:vcall, :f2]],
    "Ruby2Ruby"   => "undef :x\nundef :y\nundef :z\nf2\n",
  },

  "undef_block_wtf"  => {
    "Ruby"        => "f1\nundef :x, :y, :z\nf2",
    "ParseTree"   => [:block,
                      [:vcall, :f1],
                      [:block,
                       [:undef, [:lit, :x]],
                       [:undef, [:lit, :y]],
                       [:undef, [:lit, :z]]],
                      [:vcall, :f2]],
    "Ruby2Ruby"   => "f1\n(undef :x\nundef :y\nundef :z)\nf2\n",
  },

  "until_post"  => {
    "Ruby"        => "begin\n  (1 + 1)\nend until false",
    "ParseTree"   => [:until, [:false],
                      [:call, [:lit, 1], :+, [:array, [:lit, 1]]], false],
  },

  "until_post_not"  => {
    "Ruby"        => "begin\n  (1 + 1)\nend until not true",
    "ParseTree"   => [:while, [:true],
                      [:call, [:lit, 1], :+, [:array, [:lit, 1]]], false],
    "Ruby2Ruby"   => "begin\n  (1 + 1)\nend while true",
  },

  "until_pre"  => {
    "Ruby"        => "until false do\n  (1 + 1)\nend",
    "ParseTree"   => [:until, [:false],
                      [:call, [:lit, 1], :+, [:array, [:lit, 1]]], true],
  },

  "until_pre_mod"  => {
    "Ruby"        => "(1 + 1) until false",
    "ParseTree"   => [:until, [:false],
                      [:call, [:lit, 1], :+, [:array, [:lit, 1]]], true],
    "Ruby2Ruby"   => "until false do\n  (1 + 1)\nend",
  },

  "until_pre_not"  => {
    "Ruby"        => "until not true do\n  (1 + 1)\nend",
    "ParseTree"   => [:while, [:true],
                      [:call, [:lit, 1], :+, [:array, [:lit, 1]]], true],
    "Ruby2Ruby"   => "while true do\n  (1 + 1)\nend",
  },

  "until_pre_not_mod"  => {
    "Ruby"        => "(1 + 1) until not true",
    "ParseTree"   => [:while, [:true],
                      [:call, [:lit, 1], :+, [:array, [:lit, 1]]], true],
    "Ruby2Ruby"   => "while true do\n  (1 + 1)\nend",
  },

  "valias"  => {
    "Ruby"        => "alias $y $x",
    "ParseTree"   => [:valias, :$y, :$x],
  },

  "vcall" => {
    "Ruby"        => "method",
    "ParseTree"   => [:vcall, :method],
  },

  "while_post" => {
    "Ruby"        => "begin\n  (1 + 1)\nend while false",
    "ParseTree"   => [:while, [:false],
                      [:call, [:lit, 1], :+, [:array, [:lit, 1]]], false],
  },

  "while_post_not" => {
    "Ruby"        => "begin\n  (1 + 1)\nend while not true",
    "ParseTree"   => [:until, [:true],
                      [:call, [:lit, 1], :+, [:array, [:lit, 1]]], false],
    "Ruby2Ruby"   => "begin\n  (1 + 1)\nend until true",
  },

  "while_pre" => {
    "Ruby"        => "while false do\n  (1 + 1)\nend",
    "ParseTree"   => [:while, [:false],
                      [:call, [:lit, 1], :+, [:array, [:lit, 1]]], true],
  },

  "while_pre_mod"  => {
    "Ruby"        => "(1 + 1) while false",
    "ParseTree"   => [:while, [:false],
                      [:call, [:lit, 1], :+, [:array, [:lit, 1]]], true],
    "Ruby2Ruby"   => "while false do\n  (1 + 1)\nend", # FIX can be one liner
  },

  "while_pre_nil" => {
    "Ruby"        => "while false do\nend",
    "ParseTree"   => [:while, [:false], nil, true],
  },

  "while_pre_not" => {
    "Ruby"        => "while not true do\n  (1 + 1)\nend",
    "ParseTree"   => [:until, [:true],
                      [:call, [:lit, 1], :+, [:array, [:lit, 1]]], true],
    "Ruby2Ruby"   => "until true do\n  (1 + 1)\nend",
  },

  "while_pre_not_mod"  => {
    "Ruby"        => "(1 + 1) while not true",
    "ParseTree"   => [:until, [:true],
                      [:call, [:lit, 1], :+, [:array, [:lit, 1]]], true],
    "Ruby2Ruby"   => "until true do\n  (1 + 1)\nend", # FIX
  },

  "xstr" => {
    "Ruby"        => "`touch 5`",
    "ParseTree"   => [:xstr, 'touch 5'],
  },

  "yield"  => {
    "Ruby"        => "yield",
    "ParseTree"   => [:yield],
  },

  "yield_arg"  => {
    "Ruby"        => "yield(42)",
    "ParseTree"   => [:yield, [:lit, 42]],
  },

  "yield_args"  => {
    "Ruby"        => "yield(42, 24)",
    "ParseTree"   => [:yield, [:array, [:lit, 42], [:lit, 24]]],
  },

  "zarray" => {
    "Ruby"        => "a = []",
    "ParseTree"   => [:lasgn, :a, [:zarray]],
  },

  "zsuper"  => {
    "Ruby"        => "def x\n  super\nend",
    "ParseTree"   => [:defn, :x, [:scope, [:block, [:args], [:zsuper]]]],
  },
}

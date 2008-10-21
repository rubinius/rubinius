def test_case
{"RawParseTree"=>
  [:block,
   [:lasgn, :s, [:call, [:const, :Struct], :new, [:array, [:lit, :var]]]],
   [:lasgn, :c, [:call, [:lvar, :s], :new, [:array, [:nil]]]],
   [:op_asgn2, [:lvar, :c], :var=, :"||", [:lit, 20]],
   [:op_asgn2, [:lvar, :c], :var=, :"&&", [:lit, 21]],
   [:op_asgn2, [:lvar, :c], :var=, :+, [:lit, 22]],
   [:op_asgn2, [:call, [:call, [:lvar, :c], :d], :e], :f=, :"||", [:lit, 42]]],
 "Ruby"=>
  "s = Struct.new(:var)\nc = s.new(nil)\nc.var ||= 20\nc.var &&= 21\nc.var += 22\nc.d.e.f ||= 42\n",
 "RubyParser"=>
  s(:block,
   s(:lasgn,
    :s,
    s(:call, s(:const, :Struct), :new, s(:arglist, s(:lit, :var)))),
   s(:lasgn, :c, s(:call, s(:lvar, :s), :new, s(:arglist, s(:nil)))),
   s(:op_asgn2, s(:lvar, :c), :var=, :"||", s(:lit, 20)),
   s(:op_asgn2, s(:lvar, :c), :var=, :"&&", s(:lit, 21)),
   s(:op_asgn2, s(:lvar, :c), :var=, :+, s(:lit, 22)),
   s(:op_asgn2,
    s(:call, s(:call, s(:lvar, :c), :d, s(:arglist)), :e, s(:arglist)),
    :f=,
    :"||",
    s(:lit, 42)))}
end

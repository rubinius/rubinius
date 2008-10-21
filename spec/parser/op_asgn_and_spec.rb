def test_case
{"RawParseTree"=>
  [:block,
   [:lasgn, :a, [:lit, 0]],
   [:op_asgn_and, [:lvar, :a], [:lasgn, :a, [:lit, 2]]]],
 "Ruby"=>"a = 0\na &&= 2\n",
 "RubyParser"=>
  s(:block,
   s(:lasgn, :a, s(:lit, 0)),
   s(:op_asgn_and, s(:lvar, :a), s(:lasgn, :a, s(:lit, 2))))}
end

def test_case
{"RawParseTree"=>
  [:block,
   [:lasgn, :a, [:lit, 0]],
   [:op_asgn_or, [:lvar, :a], [:lasgn, :a, [:lit, 1]]]],
 "Ruby"=>"a = 0\na ||= 1\n",
 "ParseTree"=>
  s(:block,
   s(:lasgn, :a, s(:lit, 0)),
   s(:op_asgn_or, s(:lvar, :a), s(:lasgn, :a, s(:lit, 1))))}
end

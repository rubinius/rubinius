def test_case
{"RawParseTree"=>
  [:block,
   [:lasgn, :b, [:zarray]],
   [:op_asgn1, [:lvar, :b], [:array, [:lit, 1]], :"||", [:lit, 10]],
   [:op_asgn1, [:lvar, :b], [:array, [:lit, 2]], :"&&", [:lit, 11]],
   [:op_asgn1, [:lvar, :b], [:array, [:lit, 3]], :+, [:lit, 12]]],
 "Ruby"=>"b = []\nb[1] ||= 10\nb[2] &&= 11\nb[3] += 12\n",
 "ParseTree"=>
  s(:block,
   s(:lasgn, :b, s(:array)),
   s(:op_asgn1, s(:lvar, :b), s(:arglist, s(:lit, 1)), :"||", s(:lit, 10)),
   s(:op_asgn1, s(:lvar, :b), s(:arglist, s(:lit, 2)), :"&&", s(:lit, 11)),
   s(:op_asgn1, s(:lvar, :b), s(:arglist, s(:lit, 3)), :+, s(:lit, 12)))}
end

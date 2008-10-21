def test_case
{"RawParseTree"=>
  [:block,
   [:iasgn, :@b, [:zarray]],
   [:op_asgn1, [:ivar, :@b], [:array, [:lit, 1]], :"||", [:lit, 10]],
   [:op_asgn1, [:ivar, :@b], [:array, [:lit, 2]], :"&&", [:lit, 11]],
   [:op_asgn1, [:ivar, :@b], [:array, [:lit, 3]], :+, [:lit, 12]]],
 "Ruby"=>"@b = []\n@b[1] ||= 10\n@b[2] &&= 11\n@b[3] += 12\n",
 "RubyParser"=>
  s(:block,
   s(:iasgn, :@b, s(:array)),
   s(:op_asgn1, s(:ivar, :@b), s(:arglist, s(:lit, 1)), :"||", s(:lit, 10)),
   s(:op_asgn1, s(:ivar, :@b), s(:arglist, s(:lit, 2)), :"&&", s(:lit, 11)),
   s(:op_asgn1, s(:ivar, :@b), s(:arglist, s(:lit, 3)), :+, s(:lit, 12)))}
end

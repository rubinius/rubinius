def test_case
{"RawParseTree"=>
  [:block,
   [:lasgn, :a, [:zarray]],
   [:attrasgn, [:lvar, :a], :[]=, [:array, [:lit, 42], [:lit, 24]]]],
 "Ruby"=>"a = []; a [42] = 24",
 "ParseTree"=>
  s(:block,
   s(:lasgn, :a, s(:array)),
   s(:attrasgn, s(:lvar, :a), :[]=, s(:arglist, s(:lit, 42), s(:lit, 24)))),
 "Ruby2Ruby"=>"a = []\na[42] = 24\n"}
end

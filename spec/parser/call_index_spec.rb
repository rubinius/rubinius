def test_case
{"RawParseTree"=>
  [:block,
   [:lasgn, :a, [:zarray]],
   [:call, [:lvar, :a], :[], [:array, [:lit, 42]]]],
 "Ruby"=>"a = []\na[42]\n",
 "ParseTree"=>
  s(:block,
   s(:lasgn, :a, s(:array)),
   s(:call, s(:lvar, :a), :[], s(:arglist, s(:lit, 42))))}
end

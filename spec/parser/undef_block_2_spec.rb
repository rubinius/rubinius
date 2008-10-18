def test_case
{"RawParseTree"=>
  [:block,
   [:vcall, :f1],
   [:block, [:undef, [:lit, :x]], [:undef, [:lit, :y]]]],
 "Ruby"=>"f1\nundef :x, :y",
 "ParseTree"=>
  s(:block,
   s(:call, nil, :f1, s(:arglist)),
   s(:block, s(:undef, s(:lit, :x)), s(:undef, s(:lit, :y)))),
 "Ruby2Ruby"=>"f1\n(undef :x\nundef :y)\n"}
end

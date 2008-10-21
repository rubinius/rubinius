def test_case
{"RawParseTree"=>
  [:while, [:false], [:call, [:lit, 1], :+, [:array, [:lit, 1]]], true],
 "Ruby"=>"(1 + 1) while false",
 "RubyParser"=>
  s(:while,
   s(:false),
   s(:call, s(:lit, 1), :+, s(:arglist, s(:lit, 1))),
   true),
 "Ruby2Ruby"=>"while false do\n  (1 + 1)\nend"}
end

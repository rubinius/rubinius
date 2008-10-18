def test_case
{"RawParseTree"=>
  [:while, [:false], [:call, [:lit, 1], :+, [:array, [:lit, 1]]], false],
 "Ruby"=>"begin\n  (1 + 1)\nend while false",
 "ParseTree"=>
  s(:while,
   s(:false),
   s(:call, s(:lit, 1), :+, s(:arglist, s(:lit, 1))),
   false)}
end

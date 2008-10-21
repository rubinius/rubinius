def test_case
{"RawParseTree"=>
  [:until, [:false], [:call, [:lit, 1], :+, [:array, [:lit, 1]]], true],
 "Ruby"=>"(1 + 1) until false",
 "RubyParser"=>
  s(:until,
   s(:false),
   s(:call, s(:lit, 1), :+, s(:arglist, s(:lit, 1))),
   true),
 "Ruby2Ruby"=>"until false do\n  (1 + 1)\nend"}
end

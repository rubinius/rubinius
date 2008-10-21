def test_case
{"RawParseTree"=>
  [:until, [:false], [:call, [:lit, 1], :+, [:array, [:lit, 1]]], false],
 "Ruby"=>"begin\n  (1 + 1)\nend until false",
 "RubyParser"=>
  s(:until,
   s(:false),
   s(:call, s(:lit, 1), :+, s(:arglist, s(:lit, 1))),
   false)}
end

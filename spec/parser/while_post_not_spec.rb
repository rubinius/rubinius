def test_case
{"RawParseTree"=>
  [:until, [:true], [:call, [:lit, 1], :+, [:array, [:lit, 1]]], false],
 "Ruby"=>"begin\n  (1 + 1)\nend while not true",
 "RubyParser"=>
  s(:until,
   s(:true),
   s(:call, s(:lit, 1), :+, s(:arglist, s(:lit, 1))),
   false),
 "Ruby2Ruby"=>"begin\n  (1 + 1)\nend until true"}
end

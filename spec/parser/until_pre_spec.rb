def test_case
{"RawParseTree"=>
  [:until, [:false], [:call, [:lit, 1], :+, [:array, [:lit, 1]]], true],
 "Ruby"=>"until false do\n  (1 + 1)\nend",
 "RubyParser"=>
  s(:until,
   s(:false),
   s(:call, s(:lit, 1), :+, s(:arglist, s(:lit, 1))),
   true)}
end

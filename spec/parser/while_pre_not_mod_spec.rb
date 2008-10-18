def test_case
{"RawParseTree"=>
  [:until, [:true], [:call, [:lit, 1], :+, [:array, [:lit, 1]]], true],
 "Ruby"=>"(1 + 1) while not true",
 "ParseTree"=>
  s(:until, s(:true), s(:call, s(:lit, 1), :+, s(:arglist, s(:lit, 1))), true),
 "Ruby2Ruby"=>"until true do\n  (1 + 1)\nend"}
end

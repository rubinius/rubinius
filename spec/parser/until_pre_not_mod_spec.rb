def test_case
{"RawParseTree"=>
  [:while, [:true], [:call, [:lit, 1], :+, [:array, [:lit, 1]]], true],
 "Ruby"=>"(1 + 1) until not true",
 "ParseTree"=>
  s(:while, s(:true), s(:call, s(:lit, 1), :+, s(:arglist, s(:lit, 1))), true),
 "Ruby2Ruby"=>"while true do\n  (1 + 1)\nend"}
end

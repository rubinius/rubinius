def test_case
{"RawParseTree"=>[:begin, [:call, [:lit, 1], :+, [:array, [:lit, 1]]]],
 "Ruby"=>"begin\n  (1 + 1)\nend",
 "ParseTree"=>s(:call, s(:lit, 1), :+, s(:arglist, s(:lit, 1))),
 "Ruby2Ruby"=>"(1 + 1)"}
end

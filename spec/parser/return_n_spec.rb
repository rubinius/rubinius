def test_case
{"RawParseTree"=>[:return, [:array, [:lit, 1], [:lit, 2], [:lit, 3]]],
 "Ruby"=>"return 1, 2, 3",
 "ParseTree"=>s(:return, s(:array, s(:lit, 1), s(:lit, 2), s(:lit, 3))),
 "Ruby2Ruby"=>"return [1, 2, 3]"}
end

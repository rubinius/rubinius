def test_case
{"RawParseTree"=>[:hash, [:lit, 1], [:lit, 2], [:lit, 3], [:lit, 4]],
 "Ruby"=>"{ 1 => 2, 3 => 4 }",
 "ParseTree"=>s(:hash, s(:lit, 1), s(:lit, 2), s(:lit, 3), s(:lit, 4))}
end

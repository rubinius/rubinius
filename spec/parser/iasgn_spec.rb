def test_case
{"RawParseTree"=>[:iasgn, :@a, [:lit, 4]],
 "Ruby"=>"@a = 4",
 "ParseTree"=>s(:iasgn, :@a, s(:lit, 4))}
end

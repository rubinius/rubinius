def test_case
{"RawParseTree"=>[:match3, [:regex, "x", 0], [:str, "blah"]],
 "Ruby"=>"\"blah\" =~ /x/",
 "ParseTree"=>s(:match3, s(:lit, /x/), s(:str, "blah"))}
end

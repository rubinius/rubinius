def test_case
{"RawParseTree"=>[:str, "before after"],
 "Ruby"=>"\"before\" \\\n  \" after\"",
 "ParseTree"=>s(:str, "before after"),
 "Ruby2Ruby"=>"\"before after\""}
end

def test_case
{"RawParseTree"=>[:str, "file = (string)\n"],
 "Ruby"=>"\"file = \#{__FILE__}\\n\"",
 "ParseTree"=>s(:str, "file = (string)\n"),
 "Ruby2Ruby"=>"\"file = (string)\\n\""}
end

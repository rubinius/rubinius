def test_case
{"RawParseTree"=>[:str, "  blah\nblah\n\n"],
 "Ruby"=>"<<-EOM\n  blah\nblah\n\n  EOM",
 "ParseTree"=>s(:str, "  blah\nblah\n\n"),
 "Ruby2Ruby"=>"\"  blah\\nblah\\n\\n\""}
end

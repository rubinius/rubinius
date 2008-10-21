def test_case
{"RawParseTree"=>[:str, "  blah\nblah\n"],
 "Ruby"=>"<<'EOM'\n  blah\nblah\nEOM",
 "RubyParser"=>s(:str, "  blah\nblah\n"),
 "Ruby2Ruby"=>"\"  blah\\nblah\\n\""}
end

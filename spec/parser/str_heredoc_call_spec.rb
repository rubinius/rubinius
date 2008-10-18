def test_case
{"RawParseTree"=>[:call, [:str, "  blah\nblah\n"], :strip],
 "Ruby"=>"<<'EOM'.strip\n  blah\nblah\nEOM",
 "ParseTree"=>s(:call, s(:str, "  blah\nblah\n"), :strip, s(:arglist)),
 "Ruby2Ruby"=>"\"  blah\\nblah\\n\".strip"}
end

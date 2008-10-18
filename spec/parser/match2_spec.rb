def test_case
{"RawParseTree"=>[:match2, [:lit, /x/], [:str, "blah"]],
 "Ruby"=>"/x/ =~ \"blah\"",
 "ParseTree"=>s(:match2, s(:lit, /x/), s(:str, "blah"))}
end

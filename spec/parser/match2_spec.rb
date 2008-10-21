def test_case
{"RawParseTree"=>[:match2, [:regex, "x", 0], [:str, "blah"]],
 "Ruby"=>"/x/ =~ \"blah\"",
 "RubyParser"=>s(:match2, s(:lit, /x/), s(:str, "blah")),
 "SydneyParser"=>s(:match2, s(:regex, "x", 0), s(:str, "blah"))}
end

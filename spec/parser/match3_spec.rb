def test_case
{"RawParseTree"=>[:match3, [:regex, "x", 0], [:str, "blah"]],
 "Ruby"=>"\"blah\" =~ /x/",
 "RubyParser"=>s(:match3, s(:lit, /x/), s(:str, "blah")),
 "SydneyParser"=>s(:match3, s(:regex, "x", 0), s(:str, "blah"))}
end

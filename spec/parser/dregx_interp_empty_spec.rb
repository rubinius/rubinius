def test_case
{"RawParseTree"=>[:dregx, "a", [:evstr], [:str, "b"]],
 "Ruby"=>"/a\#{}b/",
 "RubyParser"=>s(:dregx, "a", s(:evstr), s(:str, "b"))}
end

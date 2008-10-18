def test_case
{"RawParseTree"=>[:dregx, "a", [:evstr], [:str, "b"]],
 "Ruby"=>"/a\#{}b/",
 "ParseTree"=>s(:dregx, "a", s(:evstr), s(:str, "b"))}
end

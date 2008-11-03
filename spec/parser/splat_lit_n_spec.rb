def test_case
{"RawParseTree"=>[:argscat, [:array, [:lit, 1]], [:lit, 2]],
 "Ruby"=>"[1, *2]",
 "RubyParser"=>s(:array, s(:lit, 1), s(:splat, s(:lit, 2)))}
end

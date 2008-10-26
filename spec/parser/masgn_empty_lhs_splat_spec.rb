def test_case
{"RawParseTree"=>[:masgn, [:splat], [:array, [:lit, 1], [:lit, 2]]],
 "Ruby"=>"* = 1, 2",
 "RubyParser"=>s(:masgn, s(:array, s(:splat)), s(:array, s(:lit, 1), s(:lit, 2)))}
end

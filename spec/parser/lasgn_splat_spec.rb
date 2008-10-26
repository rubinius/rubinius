def test_case
{"RawParseTree"=>[:lasgn, :a, [:svalue, [:splat, [:array, [:lit, 1]]]]],
 "Ruby"=>"a = *[1]",
 "RubyParser"=>s(:lasgn, :a, s(:svalue, s(:splat, s(:array, s(:lit, 1)))))}
end

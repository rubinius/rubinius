def test_case
{"RawParseTree"=>[:lasgn, :x, [:svalue, [:splat, [:array, [:lit, 1]]]]],
 "Ruby"=>"x = *[1]",
 "RubyParser"=>s(:lasgn, :x, s(:svalue, s(:splat, s(:array, s(:lit, 1)))))}
end

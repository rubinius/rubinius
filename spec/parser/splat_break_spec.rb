def test_case
{"RawParseTree"=>[:break, [:svalue, [:splat, [:array, [:lit, 1]]]]],
 "Ruby"=>"break *[1]",
 "RubyParser"=>s(:break, s(:svalue, s(:splat, s(:array, s(:lit, 1)))))}
end

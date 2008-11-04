def test_case
{"RawParseTree"=>[:next, [:svalue, [:splat, [:array, [:lit, 1]]]]],
 "Ruby"=>"next *[1]",
 "RubyParser"=>s(:next, s(:svalue, s(:splat, s(:array, s(:lit, 1)))))}
end

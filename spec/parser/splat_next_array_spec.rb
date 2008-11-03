def test_case
{"RawParseTree"=>[:next, [:splat, [:array, [:lit, 1]]]],
 "Ruby"=>"next [*[1]]",
 "RubyParser"=>s(:next, s(:array, s(:splat, s(:array, s(:lit, 1)))))}
end

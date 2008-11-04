def test_case
{"RawParseTree"=>[:break, [:splat, [:array, [:lit, 1]]]],
 "Ruby"=>"break [*[1]]",
 "RubyParser"=>s(:break, s(:array, s(:splat, s(:array, s(:lit, 1)))))}
end

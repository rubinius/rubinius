def test_case
{"RawParseTree"=>[:return, [:splat, [:array, [:lit, 1]]]],
 "Ruby"=>"return [*[1]]",
 "RubyParser"=>s(:return, s(:array, s(:splat, s(:array, s(:lit, 1)))))}
end

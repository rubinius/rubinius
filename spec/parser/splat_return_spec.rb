def test_case
{"RawParseTree"=>[:return, [:svalue, [:splat, [:array, [:lit, 1]]]]],
 "Ruby"=>"return *[1]",
 "RubyParser"=>s(:return, s(:svalue, s(:splat, s(:array, s(:lit, 1)))))}
end

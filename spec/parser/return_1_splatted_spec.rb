def test_case
{"RawParseTree"=>[:return, [:svalue, [:splat, [:lit, 1]]]],
 "Ruby"=>"return *1",
 "RubyParser"=>s(:return, s(:svalue, s(:splat, s(:lit, 1))))}
end

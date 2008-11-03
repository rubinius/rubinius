def test_case
{"RawParseTree"=>[:splat, [:lit, 1]],
 "Ruby"=>"[*1]",
 "RubyParser"=>s(:array, s(:splat, s(:lit, 1)))}
end

def test_case
{"RawParseTree"=>[:splat, [:array, [:lit, 1]]],
 "Ruby"=>"[*[1]]",
 "RubyParser"=>s(:array, s(:splat, s(:array, s(:lit, 1))))}
end

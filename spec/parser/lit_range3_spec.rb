def test_case
{"RawParseTree"=>[:dot3, [:lit, 1], [:lit, 10]],
 "Ruby"=>"(1...10)",
 "RubyParser"=>s(:lit, 1...10),
 "SydneyParser"=>s(:dot3, s(:lit, 1), s(:lit, 10))}
end

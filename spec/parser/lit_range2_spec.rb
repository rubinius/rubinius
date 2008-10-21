def test_case
{"RawParseTree"=>[:dot2, [:lit, 1], [:lit, 10]],
 "Ruby"=>"(1..10)",
 "RubyParser"=>s(:lit, 1..10),
 "SydneyParser"=>s(:dot2, s(:lit, 1), s(:lit, 10))}
end

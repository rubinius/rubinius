def test_case
{"RawParseTree"=>[:yield, [:array, [:lit, 42], [:lit, 24]], true],
 "Ruby"=>"yield(42, 24)",
 "RubyParser"=>s(:yield, s(:lit, 42), s(:lit, 24))}
end

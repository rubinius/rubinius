def test_case
{"RawParseTree"=>[:yield, [:lit, 42]],
 "Ruby"=>"yield(42)",
 "RubyParser"=>s(:yield, s(:lit, 42))}
end

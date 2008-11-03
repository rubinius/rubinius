def test_case
{"RawParseTree"=>[:yield, [:array, [:lit, 42]], false],
 "Ruby"=>"yield([42])",
 "RubyParser"=>s(:yield, s(:array, s(:lit, 42)))}
end

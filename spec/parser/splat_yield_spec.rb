def test_case
{"RawParseTree"=>[:yield, [:splat, [:array, [:lit, 1]]], true],
 "Ruby"=>"yield(*[1])",
 "RubyParser"=>s(:yield, s(:array, s(:splat, s(:array, s(:lit, 1)))))}
end

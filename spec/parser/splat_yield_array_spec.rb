def test_case
{"RawParseTree"=>[:yield, [:splat, [:array, [:lit, 1]]], false],
 "Ruby"=>"yield([*[1]])",
 "RubyParser"=>s(:yield, s(:array, s(:splat, s(:array, s(:lit, 1))))),
 "Ruby2Ruby"=>"yield(*[1])"}
end

def test_case
{"RawParseTree"=>[:super, [:array, [:splat, [:array, [:lit, 1]]]]],
 "Ruby"=>"super([*[1]])",
 "RubyParser"=>s(:super, s(:array, s(:splat, s(:array, s(:lit, 1))))),
 "Ruby2Ruby"=>"super(*[1])"}
end

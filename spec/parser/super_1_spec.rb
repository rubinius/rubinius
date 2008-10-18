def test_case
{"RawParseTree"=>
  [:defn, :x, [:scope, [:block, [:args], [:super, [:array, [:lit, 4]]]]]],
 "Ruby"=>"def x\n  super(4)\nend",
 "ParseTree"=>
  s(:defn, :x, s(:args), s(:scope, s(:block, s(:super, s(:lit, 4)))))}
end

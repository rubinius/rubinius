def test_case
{"RawParseTree"=>
  [:defn,
   :x,
   [:scope, [:block, [:args], [:super, [:array, [:lit, 24], [:lit, 42]]]]]],
 "Ruby"=>"def x\n  super(24, 42)\nend",
 "RubyParser"=>
  s(:defn,
   :x,
   s(:args),
   s(:scope, s(:block, s(:super, s(:lit, 24), s(:lit, 42)))))}
end

def test_case
{"RawParseTree"=>
  [:defn, :x, [:scope, [:block, [:args], [:cvasgn, :@@blah, [:lit, 1]]]]],
 "Ruby"=>"def x\n  @@blah = 1\nend",
 "RubyParser"=>
  s(:defn,
   :x,
   s(:args),
   s(:scope, s(:block, s(:cvasgn, :@@blah, s(:lit, 1)))))}
end

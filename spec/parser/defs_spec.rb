def test_case
{"RawParseTree"=>
  [:defs,
   [:self],
   :x,
   [:scope,
    [:block, [:args, :y], [:call, [:lvar, :y], :+, [:array, [:lit, 1]]]]]],
 "Ruby"=>"def self.x(y)\n  (y + 1)\nend",
 "RubyParser"=>
  s(:defs,
   s(:self),
   :x,
   s(:args, :y),
   s(:scope, s(:block, s(:call, s(:lvar, :y), :+, s(:arglist, s(:lit, 1))))))}
end

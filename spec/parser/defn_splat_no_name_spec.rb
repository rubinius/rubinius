def test_case
{"RawParseTree"=>
  [:defn,
   :x,
   [:scope, [:block, [:args, :a, :*], [:fcall, :p, [:array, [:lvar, :a]]]]]],
 "Ruby"=>"def x(a, *)\n  p(a)\nend",
 "RubyParser"=>
  s(:defn,
   :x,
   s(:args, :a, :*),
   s(:scope, s(:block, s(:call, nil, :p, s(:arglist, s(:lvar, :a))))))}
end

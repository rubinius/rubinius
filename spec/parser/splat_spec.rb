def test_case
{"RawParseTree"=>
  [:defn,
   :x,
   [:scope, [:block, [:args, :"*b"], [:fcall, :a, [:splat, [:lvar, :b]]]]]],
 "Ruby"=>"def x(*b)\n  a(*b)\nend",
 "ParseTree"=>
  s(:defn,
   :x,
   s(:args, :"*b"),
   s(:scope,
    s(:block, s(:call, nil, :a, s(:arglist, s(:splat, s(:lvar, :b)))))))}
end

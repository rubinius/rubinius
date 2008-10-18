def test_case
{"RawParseTree"=>
  [:defn,
   :x,
   [:scope,
    [:block,
     [:args, :a, :"*args"],
     [:fcall, :p, [:array, [:lvar, :a], [:lvar, :args]]]]]],
 "Ruby"=>"def x(a, *args)\n  p(a, args)\nend",
 "ParseTree"=>
  s(:defn,
   :x,
   s(:args, :a, :"*args"),
   s(:scope,
    s(:block, s(:call, nil, :p, s(:arglist, s(:lvar, :a), s(:lvar, :args))))))}
end

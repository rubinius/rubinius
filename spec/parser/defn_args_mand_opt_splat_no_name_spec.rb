def test_case
{"RawParseTree"=>
  [:defn,
   :x,
   [:scope,
    [:block,
     [:args, :a, :b, :*, [:block, [:lasgn, :b, [:lit, 42]]]],
     [:nil]]]],
 "Ruby"=>"def x(a, b = 42, *)\n  # do nothing\nend",
 "ParseTree"=>
  s(:defn,
   :x,
   s(:args, :a, :b, :*, s(:block, s(:lasgn, :b, s(:lit, 42)))),
   s(:scope, s(:block, s(:nil))))}
end

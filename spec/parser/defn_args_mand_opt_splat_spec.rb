def test_case
{"RawParseTree"=>
  [:defn,
   :f,
   [:scope,
    [:block,
     [:args, :mand, :opt, :"*rest", [:block, [:lasgn, :opt, [:lit, 42]]]],
     [:nil]]]],
 "Ruby"=>"def f(mand, opt = 42, *rest)\n  # do nothing\nend",
 "ParseTree"=>
  s(:defn,
   :f,
   s(:args, :mand, :opt, :"*rest", s(:block, s(:lasgn, :opt, s(:lit, 42)))),
   s(:scope, s(:block, s(:nil))))}
end

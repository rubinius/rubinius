def test_case
{"RawParseTree"=>
  [:defn,
   :f,
   [:scope,
    [:block,
     [:args, :mand, :opt, [:block, [:lasgn, :opt, [:lit, 42]]]],
     [:nil]]]],
 "Ruby"=>"def f(mand, opt = 42)\n  # do nothing\nend",
 "ParseTree"=>
  s(:defn,
   :f,
   s(:args, :mand, :opt, s(:block, s(:lasgn, :opt, s(:lit, 42)))),
   s(:scope, s(:block, s(:nil))))}
end

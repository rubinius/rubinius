def test_case
{"RawParseTree"=>
  [:defn,
   :f,
   [:scope,
    [:block, [:args, :opt, [:block, [:lasgn, :opt, [:lit, 42]]]], [:nil]]]],
 "Ruby"=>"def f(opt = 42)\n  # do nothing\nend",
 "ParseTree"=>
  s(:defn,
   :f,
   s(:args, :opt, s(:block, s(:lasgn, :opt, s(:lit, 42)))),
   s(:scope, s(:block, s(:nil))))}
end

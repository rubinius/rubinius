def test_case
{"RawParseTree"=>
  [:defn,
   :f,
   [:scope,
    [:block,
     [:args, :opt, :"*rest", [:block, [:lasgn, :opt, [:lit, 42]]]],
     [:nil]]]],
 "Ruby"=>"def f(opt = 42, *rest)\n  # do nothing\nend",
 "RubyParser"=>
  s(:defn,
   :f,
   s(:args, :opt, :"*rest", s(:block, s(:lasgn, :opt, s(:lit, 42)))),
   s(:scope, s(:block, s(:nil))))}
end

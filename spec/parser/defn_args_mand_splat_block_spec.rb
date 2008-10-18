def test_case
{"RawParseTree"=>
  [:defn,
   :f,
   [:scope, [:block, [:args, :mand, :"*rest"], [:block_arg, :block], [:nil]]]],
 "Ruby"=>"def f(mand, *rest, &block)\n  # do nothing\nend",
 "ParseTree"=>
  s(:defn,
   :f,
   s(:args, :mand, :"*rest", :"&block"),
   s(:scope, s(:block, s(:nil))))}
end

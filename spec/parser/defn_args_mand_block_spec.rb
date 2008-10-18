def test_case
{"RawParseTree"=>
  [:defn,
   :f,
   [:scope, [:block, [:args, :mand], [:block_arg, :block], [:nil]]]],
 "Ruby"=>"def f(mand, &block)\n  # do nothing\nend",
 "ParseTree"=>
  s(:defn, :f, s(:args, :mand, :"&block"), s(:scope, s(:block, s(:nil))))}
end

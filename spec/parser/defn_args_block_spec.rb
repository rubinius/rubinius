def test_case
{"RawParseTree"=>
  [:defn, :f, [:scope, [:block, [:args], [:block_arg, :block], [:nil]]]],
 "Ruby"=>"def f(&block)\n  # do nothing\nend",
 "ParseTree"=>s(:defn, :f, s(:args, :"&block"), s(:scope, s(:block, s(:nil))))}
end

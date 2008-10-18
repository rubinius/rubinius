def test_case
{"RawParseTree"=>[:defn, :x, [:scope, [:block, [:args, :*], [:nil]]]],
 "Ruby"=>"def x(*)\n  # do nothing\nend",
 "ParseTree"=>s(:defn, :x, s(:args, :*), s(:scope, s(:block, s(:nil))))}
end

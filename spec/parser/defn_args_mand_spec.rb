def test_case
{"RawParseTree"=>[:defn, :f, [:scope, [:block, [:args, :mand], [:nil]]]],
 "Ruby"=>"def f(mand)\n  # do nothing\nend",
 "ParseTree"=>s(:defn, :f, s(:args, :mand), s(:scope, s(:block, s(:nil))))}
end

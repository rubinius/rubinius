def test_case
{"RawParseTree"=>[:defn, :something?, [:scope, [:block, [:args], [:nil]]]],
 "Ruby"=>"def something?\n  # do nothing\nend",
 "ParseTree"=>s(:defn, :something?, s(:args), s(:scope, s(:block, s(:nil))))}
end

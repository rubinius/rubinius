def test_case
{"RawParseTree"=>[:defn, :|, [:scope, [:block, [:args, :o], [:nil]]]],
 "Ruby"=>"def |(o)\n  # do nothing\nend",
 "ParseTree"=>s(:defn, :|, s(:args, :o), s(:scope, s(:block, s(:nil))))}
end

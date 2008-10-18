def test_case
{"RawParseTree"=>[:defn, :m, [:scope, [:block, [:args], [:nil]]]],
 "Ruby"=>"def m\n  begin\n\n  end\nend",
 "ParseTree"=>s(:defn, :m, s(:args), s(:scope, s(:block, s(:nil)))),
 "Ruby2Ruby"=>"def m\n  # do nothing\nend"}
end

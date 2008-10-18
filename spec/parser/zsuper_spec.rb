def test_case
{"RawParseTree"=>[:defn, :x, [:scope, [:block, [:args], [:zsuper]]]],
 "Ruby"=>"def x\n  super\nend",
 "ParseTree"=>s(:defn, :x, s(:args), s(:scope, s(:block, s(:zsuper))))}
end

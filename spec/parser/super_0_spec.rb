def test_case
{"RawParseTree"=>[:defn, :x, [:scope, [:block, [:args], [:super]]]],
 "Ruby"=>"def x\n  super()\nend",
 "ParseTree"=>s(:defn, :x, s(:args), s(:scope, s(:block, s(:super))))}
end

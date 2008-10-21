def test_case
{"RawParseTree"=>[:defn, :x, [:scope, [:block, [:args], [:zsuper]]]],
 "Ruby"=>"def x\n  super\nend",
 "RubyParser"=>s(:defn, :x, s(:args), s(:scope, s(:block, s(:zsuper))))}
end

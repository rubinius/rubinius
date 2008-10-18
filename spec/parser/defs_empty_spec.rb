def test_case
{"RawParseTree"=>[:defs, [:self], :empty, [:scope, [:args]]],
 "Ruby"=>"def self.empty\n  # do nothing\nend",
 "ParseTree"=>s(:defs, s(:self), :empty, s(:args), s(:scope, s(:block)))}
end

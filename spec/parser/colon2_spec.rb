def test_case
{"RawParseTree"=>[:colon2, [:const, :X], :Y],
 "Ruby"=>"X::Y",
 "ParseTree"=>s(:colon2, s(:const, :X), :Y)}
end

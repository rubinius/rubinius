def test_case
{"RawParseTree"=>[:cdecl, [:colon2, [:const, :X], :Y], [:lit, 1]],
 "Ruby"=>"X::Y = 1",
 "ParseTree"=>s(:cdecl, s(:colon2, s(:const, :X), :Y), s(:lit, 1))}
end

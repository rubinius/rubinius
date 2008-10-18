def test_case
{"RawParseTree"=>[:cdecl, [:colon3, :X], [:lit, 1]],
 "Ruby"=>"::X = 1",
 "ParseTree"=>s(:cdecl, s(:colon3, :X), s(:lit, 1))}
end

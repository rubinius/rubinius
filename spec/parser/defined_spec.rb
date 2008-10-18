def test_case
{"RawParseTree"=>[:defined, [:gvar, :$x]],
 "Ruby"=>"defined? $x",
 "ParseTree"=>s(:defined, s(:gvar, :$x))}
end

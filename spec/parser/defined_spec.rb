def test_case
{"RawParseTree"=>[:defined, [:gvar, :$x]],
 "Ruby"=>"defined? $x",
 "RubyParser"=>s(:defined, s(:gvar, :$x))}
end

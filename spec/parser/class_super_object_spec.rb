def test_case
{"RawParseTree"=>[:class, :X, [:const, :Object], [:scope]],
 "Ruby"=>"class X < Object\nend",
 "RubyParser"=>s(:class, :X, s(:const, :Object), s(:scope))}
end

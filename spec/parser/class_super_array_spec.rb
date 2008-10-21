def test_case
{"RawParseTree"=>[:class, :X, [:const, :Array], [:scope]],
 "Ruby"=>"class X < Array\nend",
 "RubyParser"=>s(:class, :X, s(:const, :Array), s(:scope))}
end

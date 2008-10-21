def test_case
{"RawParseTree"=>[:class, :X, nil, [:scope, [:cvdecl, :@@blah, [:lit, 1]]]],
 "Ruby"=>"class X\n  @@blah = 1\nend",
 "RubyParser"=>s(:class, :X, nil, s(:scope, s(:cvdecl, :@@blah, s(:lit, 1))))}
end

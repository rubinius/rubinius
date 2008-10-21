def test_case
{"RawParseTree"=>[:sclass, [:self], [:scope, [:lit, 42]]],
 "Ruby"=>"class << self\n  42\nend",
 "RubyParser"=>s(:sclass, s(:self), s(:scope, s(:lit, 42)))}
end

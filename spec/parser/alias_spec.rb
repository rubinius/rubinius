def test_case
{"RawParseTree"=>[:class, :X, nil, [:scope, [:alias, [:lit, :y], [:lit, :x]]]],
 "Ruby"=>"class X\n  alias :y :x\nend",
 "RubyParser"=>
  s(:class, :X, nil, s(:scope, s(:alias, s(:lit, :y), s(:lit, :x)))),
 "Ruby2Ruby"=>"class X\n  alias_method :y, :x\nend"}
end

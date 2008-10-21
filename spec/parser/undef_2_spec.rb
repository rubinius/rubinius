def test_case
{"RawParseTree"=>[:block, [:undef, [:lit, :x]], [:undef, [:lit, :y]]],
 "Ruby"=>"undef :x, :y",
 "RubyParser"=>s(:block, s(:undef, s(:lit, :x)), s(:undef, s(:lit, :y))),
 "Ruby2Ruby"=>"undef :x\nundef :y\n"}
end

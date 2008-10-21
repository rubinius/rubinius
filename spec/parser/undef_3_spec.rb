def test_case
{"RawParseTree"=>
  [:block, [:undef, [:lit, :x]], [:undef, [:lit, :y]], [:undef, [:lit, :z]]],
 "Ruby"=>"undef :x, :y, :z",
 "RubyParser"=>
  s(:block,
   s(:undef, s(:lit, :x)),
   s(:undef, s(:lit, :y)),
   s(:undef, s(:lit, :z))),
 "Ruby2Ruby"=>"undef :x\nundef :y\nundef :z\n"}
end

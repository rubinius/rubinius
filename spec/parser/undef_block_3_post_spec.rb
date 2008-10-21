def test_case
{"RawParseTree"=>
  [:block,
   [:undef, [:lit, :x]],
   [:undef, [:lit, :y]],
   [:undef, [:lit, :z]],
   [:vcall, :f2]],
 "Ruby"=>"undef :x, :y, :z\nf2",
 "RubyParser"=>
  s(:block,
   s(:undef, s(:lit, :x)),
   s(:undef, s(:lit, :y)),
   s(:undef, s(:lit, :z)),
   s(:call, nil, :f2, s(:arglist))),
 "Ruby2Ruby"=>"undef :x\nundef :y\nundef :z\nf2\n"}
end

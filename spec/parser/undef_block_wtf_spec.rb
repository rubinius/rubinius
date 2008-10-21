def test_case
{"RawParseTree"=>
  [:block,
   [:vcall, :f1],
   [:block, [:undef, [:lit, :x]], [:undef, [:lit, :y]], [:undef, [:lit, :z]]],
   [:vcall, :f2]],
 "Ruby"=>"f1\nundef :x, :y, :z\nf2",
 "RubyParser"=>
  s(:block,
   s(:call, nil, :f1, s(:arglist)),
   s(:block,
    s(:undef, s(:lit, :x)),
    s(:undef, s(:lit, :y)),
    s(:undef, s(:lit, :z))),
   s(:call, nil, :f2, s(:arglist))),
 "Ruby2Ruby"=>"f1\n(undef :x\nundef :y\nundef :z)\nf2\n"}
end

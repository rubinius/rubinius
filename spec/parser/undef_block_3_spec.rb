def test_case
{"RawParseTree"=>
  [:block,
   [:vcall, :f1],
   [:block, [:undef, [:lit, :x]], [:undef, [:lit, :y]], [:undef, [:lit, :z]]]],
 "Ruby"=>"f1\nundef :x, :y, :z",
 "RubyParser"=>
  s(:block,
   s(:call, nil, :f1, s(:arglist)),
   s(:block,
    s(:undef, s(:lit, :x)),
    s(:undef, s(:lit, :y)),
    s(:undef, s(:lit, :z)))),
 "Ruby2Ruby"=>"f1\n(undef :x\nundef :y\nundef :z)\n"}
end

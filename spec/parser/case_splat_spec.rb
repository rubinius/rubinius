def test_case
{"RawParseTree"=>
  [:case,
   [:vcall, :a],
   [:when, [:array, [:lit, :b], [:when, [:vcall, :c], nil]], [:vcall, :d]],
   [:vcall, :e]],
 "Ruby"=>"case a\nwhen :b, *c then\n  d\nelse\n  e\nend",
 "RubyParser"=>
  s(:case,
   s(:call, nil, :a, s(:arglist)),
   s(:when,
    s(:array, s(:lit, :b), s(:when, s(:call, nil, :c, s(:arglist)), nil)),
    s(:call, nil, :d, s(:arglist))),
   s(:call, nil, :e, s(:arglist)))}
end

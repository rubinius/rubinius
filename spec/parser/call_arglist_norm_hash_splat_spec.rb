def test_case
{"RawParseTree"=>
  [:call,
   [:vcall, :o],
   :m,
   [:argscat,
    [:array,
     [:lit, 42],
     [:hash, [:lit, :a], [:lit, 1], [:lit, :b], [:lit, 2]]],
    [:vcall, :c]]],
 "Ruby"=>"o.m(42, :a => 1, :b => 2, *c)",
 "ParseTree"=>
  s(:call,
   s(:call, nil, :o, s(:arglist)),
   :m,
   s(:arglist,
    s(:lit, 42),
    s(:hash, s(:lit, :a), s(:lit, 1), s(:lit, :b), s(:lit, 2)),
    s(:splat, s(:call, nil, :c, s(:arglist)))))}
end

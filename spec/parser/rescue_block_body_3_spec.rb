def test_case
{"RawParseTree"=>
  [:begin,
   [:rescue,
    [:vcall, :a],
    [:resbody,
     [:array, [:const, :A]],
     [:vcall, :b],
     [:resbody,
      [:array, [:const, :B]],
      [:vcall, :c],
      [:resbody, [:array, [:const, :C]], [:vcall, :d]]]]]],
 "Ruby"=>"begin\n  a\nrescue A\n  b\nrescue B\n  c\nrescue C\n  d\nend",
 "ParseTree"=>
  s(:rescue,
   s(:call, nil, :a, s(:arglist)),
   s(:resbody, s(:array, s(:const, :A)), s(:call, nil, :b, s(:arglist))),
   s(:resbody, s(:array, s(:const, :B)), s(:call, nil, :c, s(:arglist))),
   s(:resbody, s(:array, s(:const, :C)), s(:call, nil, :d, s(:arglist))))}
end

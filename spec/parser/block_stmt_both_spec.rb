def test_case
{"RawParseTree"=>
  [:defn,
   :f,
   [:scope,
    [:block,
     [:args],
     [:vcall, :a],
     [:rescue, [:vcall, :b], [:resbody, nil, [:vcall, :c]]],
     [:vcall, :d]]]],
 "Ruby"=>"def f\n  a\n  begin\n    b\n  rescue\n    c\n  end\n  d\nend",
 "RubyParser"=>
  s(:defn,
   :f,
   s(:args),
   s(:scope,
    s(:block,
     s(:call, nil, :a, s(:arglist)),
     s(:rescue,
      s(:call, nil, :b, s(:arglist)),
      s(:resbody, s(:array), s(:call, nil, :c, s(:arglist)))),
     s(:call, nil, :d, s(:arglist))))),
 "Ruby2Ruby"=>"def f\n  a\n  b rescue c\n  d\nend"}
end

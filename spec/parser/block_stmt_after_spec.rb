def test_case
{"RawParseTree"=>
  [:defn,
   :f,
   [:scope,
    [:block,
     [:args],
     [:rescue, [:vcall, :b], [:resbody, nil, [:vcall, :c]]],
     [:vcall, :d]]]],
 "Ruby"=>"def f\n  begin\n    b\n  rescue\n    c\n  end\n\n  d\nend",
 "RubyParser"=>
  s(:defn,
   :f,
   s(:args),
   s(:scope,
    s(:block,
     s(:rescue,
      s(:call, nil, :b, s(:arglist)),
      s(:resbody, s(:array), s(:call, nil, :c, s(:arglist)))),
     s(:call, nil, :d, s(:arglist))))),
 "Ruby2Ruby"=>"def f\n  b rescue c\n  d\nend"}
end

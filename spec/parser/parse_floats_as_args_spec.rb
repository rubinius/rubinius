def test_case
{"RawParseTree"=>
  [:defn,
   :x,
   [:scope,
    [:block,
     [:args,
      :a,
      :b,
      [:block, [:lasgn, :a, [:lit, 0.0]], [:lasgn, :b, [:lit, 0.0]]]],
     [:call, [:lvar, :a], :+, [:array, [:lvar, :b]]]]]],
 "Ruby"=>"def x(a=0.0,b=0.0)\n  a+b\nend",
 "RubyParser"=>
  s(:defn,
   :x,
   s(:args,
    :a,
    :b,
    s(:block, s(:lasgn, :a, s(:lit, 0.0)), s(:lasgn, :b, s(:lit, 0.0)))),
   s(:scope,
    s(:block, s(:call, s(:lvar, :a), :+, s(:arglist, s(:lvar, :b)))))),
 "Ruby2Ruby"=>"def x(a = 0.0, b = 0.0)\n  (a + b)\nend"}
end

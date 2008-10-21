def test_case
{"RawParseTree"=>
  [:masgn,
   [:array, [:lasgn, :a], [:lasgn, :b]],
   [:lasgn, :c],
   [:to_ary, [:vcall, :d]]],
 "Ruby"=>"a, b, *c = d",
 "RubyParser"=>
  s(:masgn,
   s(:array, s(:lasgn, :a), s(:lasgn, :b), s(:splat, s(:lasgn, :c))),
   s(:to_ary, s(:call, nil, :d, s(:arglist))))}
end

def test_case
{"RawParseTree"=>
  [:masgn,
   [:array, [:lasgn, :a], [:lasgn, :b]],
   [:splat],
   [:to_ary, [:vcall, :c]]],
 "Ruby"=>"a, b, * = c",
 "RubyParser"=>
  s(:masgn,
   s(:array, s(:lasgn, :a), s(:lasgn, :b), s(:splat)),
   s(:to_ary, s(:call, nil, :c, s(:arglist))))}
end

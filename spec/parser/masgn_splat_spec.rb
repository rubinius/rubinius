def test_case
{"RawParseTree"=>
  [:masgn,
   [:array, [:lasgn, :a], [:lasgn, :b]],
   [:lasgn, :c],
   [:array, [:vcall, :d], [:vcall, :e], [:vcall, :f], [:vcall, :g]]],
 "Ruby"=>"a, b, *c = d, e, f, g",
 "ParseTree"=>
  s(:masgn,
   s(:array, s(:lasgn, :a), s(:lasgn, :b), s(:splat, s(:lasgn, :c))),
   s(:array,
    s(:call, nil, :d, s(:arglist)),
    s(:call, nil, :e, s(:arglist)),
    s(:call, nil, :f, s(:arglist)),
    s(:call, nil, :g, s(:arglist))))}
end

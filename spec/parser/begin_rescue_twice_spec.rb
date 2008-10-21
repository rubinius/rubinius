def test_case
{"RawParseTree"=>
  [:block,
   [:rescue, [:vcall, :a], [:resbody, nil, [:lasgn, :mes, [:gvar, :$!]]]],
   [:begin,
    [:rescue, [:vcall, :b], [:resbody, nil, [:lasgn, :mes, [:gvar, :$!]]]]]],
 "Ruby"=>
  "begin\n  a\nrescue => mes\n  # do nothing\nend\nbegin\n  b\nrescue => mes\n  # do nothing\nend\n",
 "RubyParser"=>
  s(:block,
   s(:rescue,
    s(:call, nil, :a, s(:arglist)),
    s(:resbody, s(:array, s(:lasgn, :mes, s(:gvar, :$!))), nil)),
   s(:rescue,
    s(:call, nil, :b, s(:arglist)),
    s(:resbody, s(:array, s(:lasgn, :mes, s(:gvar, :$!))), nil)))}
end

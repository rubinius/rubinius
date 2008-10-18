def test_case
{"RawParseTree"=>
  [:masgn,
   [:array, [:lasgn, :a], [:lasgn, :b]],
   [:lasgn, :c],
   [:to_ary, [:call, [:vcall, :d], :e, [:array, [:str, "f"]]]]],
 "Ruby"=>"a, b, *c = d.e(\"f\")",
 "ParseTree"=>
  s(:masgn,
   s(:array, s(:lasgn, :a), s(:lasgn, :b), s(:splat, s(:lasgn, :c))),
   s(:to_ary,
    s(:call, s(:call, nil, :d, s(:arglist)), :e, s(:arglist, s(:str, "f")))))}
end

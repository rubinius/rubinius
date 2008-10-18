def test_case
{"RawParseTree"=>
  [:masgn,
   [:array,
    [:attrasgn, [:vcall, :a], :b=],
    [:attrasgn, [:vcall, :a], :c=],
    [:lasgn, :_]],
   nil,
   [:to_ary, [:vcall, :q]]],
 "Ruby"=>"a.b, a.c, _ = q",
 "ParseTree"=>
  s(:masgn,
   s(:array,
    s(:attrasgn, s(:call, nil, :a, s(:arglist)), :b=, s(:arglist)),
    s(:attrasgn, s(:call, nil, :a, s(:arglist)), :c=, s(:arglist)),
    s(:lasgn, :_)),
   s(:to_ary, s(:call, nil, :q, s(:arglist))))}
end

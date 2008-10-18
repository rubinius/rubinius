def test_case
{"RawParseTree"=>
  [:masgn,
   [:array, [:lasgn, :a], [:attrasgn, [:vcall, :b], :c=]],
   nil,
   [:array, [:vcall, :d], [:vcall, :e]]],
 "Ruby"=>"a, b.c = d, e",
 "ParseTree"=>
  s(:masgn,
   s(:array,
    s(:lasgn, :a),
    s(:attrasgn, s(:call, nil, :b, s(:arglist)), :c=, s(:arglist))),
   s(:array, s(:call, nil, :d, s(:arglist)), s(:call, nil, :e, s(:arglist))))}
end

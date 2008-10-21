def test_case
{"RawParseTree"=>
  [:attrasgn,
   [:vcall, :a],
   :[]=,
   [:argspush, [:splat, [:vcall, :b]], [:vcall, :c]]],
 "Ruby"=>"a[*b] = c",
 "RubyParser"=>
  s(:attrasgn,
   s(:call, nil, :a, s(:arglist)),
   :[]=,
   s(:arglist,
    s(:splat, s(:call, nil, :b, s(:arglist))),
    s(:call, nil, :c, s(:arglist))))}
end

def test_case
{"RawParseTree"=>
  [:attrasgn,
   [:vcall, :a],
   :m=,
   [:array, [:svalue, [:splat, [:array, [:lit, 1]]]]]],
 "Ruby"=>"a.m = *[1]",
 "RubyParser"=>
  s(:attrasgn,
   s(:call, nil, :a, s(:arglist)),
   :m=,
   s(:arglist, s(:svalue, s(:splat, s(:array, s(:lit, 1))))))}
end

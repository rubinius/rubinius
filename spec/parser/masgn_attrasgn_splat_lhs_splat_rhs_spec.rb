def test_case
{"RawParseTree"=>
  [:masgn, [:attrasgn, [:vcall, :a], :m=], [:splat, [:vcall, :b]]],
 "Ruby"=>"*a.m = *b",
 "RubyParser"=>
  s(:masgn,
   s(:array,
    s(:splat, s(:attrasgn, s(:call, nil, :a, s(:arglist)), :m=, s(:arglist)))),
   s(:splat, s(:call, nil, :b, s(:arglist))))}
end

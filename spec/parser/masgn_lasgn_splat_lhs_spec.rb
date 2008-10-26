def test_case
{"RawParseTree"=>
  [:masgn, [:lasgn, :a], [:array, [:vcall, :b]]],
 "Ruby"=>"*a = b",
 "RubyParser"=>
  s(:masgn,
   s(:array, s(:splat, s(:lasgn, :a))),
   s(:to_ary, s(:call, nil, :b, s(:arglist))))}
end

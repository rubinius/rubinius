def test_case
{"RawParseTree"=>
  [:masgn, [:lasgn, :a], [:splat, [:vcall, :b]]],
 "Ruby"=>"*a = *b",
 "RubyParser"=>
  s(:masgn,
   s(:array, s(:splat, s(:lasgn, :a))),
   s(:splat, s(:call, nil, :b, s(:arglist))))}
end

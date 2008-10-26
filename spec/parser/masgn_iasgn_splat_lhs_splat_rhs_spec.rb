def test_case
{"RawParseTree"=>
  [:masgn, [:iasgn, :@a], [:splat, [:vcall, :b]]],
 "Ruby"=>"*@a = *b",
 "RubyParser"=>
  s(:masgn,
   s(:array, s(:splat, s(:iasgn, :@a))),
   s(:splat, s(:call, nil, :b, s(:arglist))))}
end

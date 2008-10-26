def test_case
{"RawParseTree"=>
  [:masgn, [:gasgn, :$a], [:splat, [:vcall, :b]]],
 "Ruby"=>"*$a = *b",
 "RubyParser"=>
  s(:masgn,
   s(:array, s(:splat, s(:gasgn, :$a))),
   s(:splat, s(:call, nil, :b, s(:arglist))))}
end

def test_case
{"RawParseTree"=>
  [:masgn, [:gasgn, :$a], [:array, [:vcall, :b]]],
 "Ruby"=>"*$a = b",
 "RubyParser"=>
  s(:masgn,
   s(:array, s(:splat, s(:gasgn, :$a))),
   s(:to_ary, s(:call, nil, :b, s(:arglist))))}
end

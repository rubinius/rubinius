def test_case
{"RawParseTree"=>
  [:masgn, [:array, [:lasgn, :a], [:lasgn, :b]], nil, [:to_ary, [:vcall, :c]]],
 "Ruby"=>"a, b = c",
 "ParseTree"=>
  s(:masgn,
   s(:array, s(:lasgn, :a), s(:lasgn, :b)),
   s(:to_ary, s(:call, nil, :c, s(:arglist))))}
end

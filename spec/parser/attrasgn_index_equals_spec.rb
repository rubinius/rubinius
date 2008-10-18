def test_case
{"RawParseTree"=>
  [:attrasgn, [:vcall, :a], :[]=, [:array, [:lit, 42], [:lit, 24]]],
 "Ruby"=>"a[42] = 24",
 "ParseTree"=>
  s(:attrasgn,
   s(:call, nil, :a, s(:arglist)),
   :[]=,
   s(:arglist, s(:lit, 42), s(:lit, 24)))}
end

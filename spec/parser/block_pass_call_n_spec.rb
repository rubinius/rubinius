def test_case
{"RawParseTree"=>
  [:block_pass,
   [:vcall, :c],
   [:call, [:vcall, :a], :b, [:array, [:lit, 1], [:lit, 2], [:lit, 3]]]],
 "Ruby"=>"a.b(1, 2, 3, &c)",
 "ParseTree"=>
  s(:call,
   s(:call, nil, :a, s(:arglist)),
   :b,
   s(:arglist,
    s(:lit, 1),
    s(:lit, 2),
    s(:lit, 3),
    s(:block_pass, s(:call, nil, :c, s(:arglist)))))}
end

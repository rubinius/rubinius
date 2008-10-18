def test_case
{"RawParseTree"=>
  [:or, [:or, [:vcall, :a], [:vcall, :b]], [:and, [:vcall, :c], [:vcall, :d]]],
 "Ruby"=>"((a || b) || (c && d))",
 "ParseTree"=>
  s(:or,
   s(:or, s(:call, nil, :a, s(:arglist)), s(:call, nil, :b, s(:arglist))),
   s(:and, s(:call, nil, :c, s(:arglist)), s(:call, nil, :d, s(:arglist)))),
 "Ruby2Ruby"=>"((a or b) or (c and d))"}
end

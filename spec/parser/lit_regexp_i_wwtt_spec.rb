def test_case
{"RawParseTree"=>[:call, [:vcall, :str], :split, [:array, [:lit, //i]]],
 "Ruby"=>"str.split(//i)",
 "ParseTree"=>
  s(:call,
   s(:call, nil, :str, s(:arglist)),
   :split,
   s(:arglist, s(:lit, //i)))}
end

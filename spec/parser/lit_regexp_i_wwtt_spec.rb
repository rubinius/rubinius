def test_case
{"RawParseTree"=>[:call, [:vcall, :str], :split, [:array, [:regex, "", 1]]],
 "Ruby"=>"str.split(//i)",
 "ParseTree"=>
  s(:call,
   s(:call, nil, :str, s(:arglist)),
   :split,
   s(:arglist, s(:lit, //i)))}
end

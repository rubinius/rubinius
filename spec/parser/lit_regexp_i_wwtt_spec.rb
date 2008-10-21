def test_case
{"RawParseTree"=>[:call, [:vcall, :str], :split, [:array, [:regex, "", 1]]],
 "Ruby"=>"str.split(//i)",
 "RubyParser"=>
  s(:call,
   s(:call, nil, :str, s(:arglist)),
   :split,
   s(:arglist, s(:lit, //i))),
 "SydneyParser"=>
  s(:call,
   s(:call, nil, :str, s(:arglist)),
   :split,
   s(:arglist, s(:regex, "", 1)))}
end

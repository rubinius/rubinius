def test_case
{"RawParseTree"=>
  [:block_pass, [:vcall, :c], [:call, [:vcall, :a], :b, [:array, [:lit, 4]]]],
 "Ruby"=>"a.b(4, &c)",
 "RubyParser"=>
  s(:call,
   s(:call, nil, :a, s(:arglist)),
   :b,
   s(:arglist, s(:lit, 4), s(:block_pass, s(:call, nil, :c, s(:arglist)))))}
end

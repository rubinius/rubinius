def test_case
{"RawParseTree"=>[:block_pass, [:vcall, :c], [:call, [:vcall, :a], :b]],
 "Ruby"=>"a.b(&c)",
 "RubyParser"=>
  s(:call,
   s(:call, nil, :a, s(:arglist)),
   :b,
   s(:arglist, s(:block_pass, s(:call, nil, :c, s(:arglist)))))}
end

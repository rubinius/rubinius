def test_case
{"RawParseTree"=>[:block_pass, [:vcall, :b], [:super, [:array, [:vcall, :a]]]],
 "Ruby"=>"super(a, &b)",
 "ParseTree"=>
  s(:super,
   s(:call, nil, :a, s(:arglist)),
   s(:block_pass, s(:call, nil, :b, s(:arglist))))}
end

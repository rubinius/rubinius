def test_case
{"RawParseTree"=>[:super, [:argscat, [:array, [:vcall, :a]], [:vcall, :b]]],
 "Ruby"=>"super(a, *b)",
 "RubyParser"=>
  s(:super,
   s(:call, nil, :a, s(:arglist)),
   s(:splat, s(:call, nil, :b, s(:arglist))))}
end

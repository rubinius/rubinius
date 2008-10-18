def test_case
{"RawParseTree"=>
  [:module, [:colon2, [:const, :X], :Y], [:scope, [:vcall, :c]]],
 "Ruby"=>"module X::Y\n  c\nend",
 "ParseTree"=>
  s(:module,
   s(:colon2, s(:const, :X), :Y),
   s(:scope, s(:call, nil, :c, s(:arglist))))}
end

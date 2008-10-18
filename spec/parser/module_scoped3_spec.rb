def test_case
{"RawParseTree"=>[:module, [:colon3, :Y], [:scope, [:vcall, :c]]],
 "Ruby"=>"module ::Y\n  c\nend",
 "ParseTree"=>
  s(:module, s(:colon3, :Y), s(:scope, s(:call, nil, :c, s(:arglist))))}
end

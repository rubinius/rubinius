def test_case
{"RawParseTree"=>[:class, [:colon3, :Y], nil, [:scope, [:vcall, :c]]],
 "Ruby"=>"class ::Y\n  c\nend",
 "ParseTree"=>
  s(:class, s(:colon3, :Y), nil, s(:scope, s(:call, nil, :c, s(:arglist))))}
end

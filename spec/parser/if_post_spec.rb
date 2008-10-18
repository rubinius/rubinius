def test_case
{"RawParseTree"=>[:if, [:vcall, :b], [:vcall, :a], nil],
 "Ruby"=>"a if b",
 "ParseTree"=>
  s(:if, s(:call, nil, :b, s(:arglist)), s(:call, nil, :a, s(:arglist)), nil)}
end

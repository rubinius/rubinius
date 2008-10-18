def test_case
{"RawParseTree"=>[:dot3, [:vcall, :a], [:vcall, :b]],
 "Ruby"=>"(a...b)",
 "ParseTree"=>
  s(:dot3, s(:call, nil, :a, s(:arglist)), s(:call, nil, :b, s(:arglist)))}
end

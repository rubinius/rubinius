def test_case
{"RawParseTree"=>[:dot2, [:vcall, :a], [:vcall, :b]],
 "Ruby"=>"(a..b)",
 "ParseTree"=>
  s(:dot2, s(:call, nil, :a, s(:arglist)), s(:call, nil, :b, s(:arglist)))}
end

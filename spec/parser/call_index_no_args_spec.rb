def test_case
{"RawParseTree"=>[:call, [:vcall, :a], :[]],
 "Ruby"=>"a[]",
 "ParseTree"=>s(:call, s(:call, nil, :a, s(:arglist)), :[], s(:arglist))}
end

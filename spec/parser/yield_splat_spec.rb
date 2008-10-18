def test_case
{"RawParseTree"=>[:yield, [:splat, [:vcall, :ary]]],
 "Ruby"=>"yield(*ary)",
 "ParseTree"=>s(:yield, s(:splat, s(:call, nil, :ary, s(:arglist))))}
end

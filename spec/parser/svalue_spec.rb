def test_case
{"RawParseTree"=>[:lasgn, :a, [:svalue, [:splat, [:vcall, :b]]]],
 "Ruby"=>"a = *b",
 "ParseTree"=>
  s(:lasgn, :a, s(:svalue, s(:splat, s(:call, nil, :b, s(:arglist)))))}
end

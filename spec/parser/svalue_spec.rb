def test_case
{"RawParseTree"=>[:lasgn, :a, [:svalue, [:splat, [:vcall, :b]]]],
 "Ruby"=>"a = *b",
 "RubyParser"=>
  s(:lasgn, :a, s(:svalue, s(:splat, s(:call, nil, :b, s(:arglist)))))}
end

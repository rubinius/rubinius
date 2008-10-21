def test_case
{"RawParseTree"=>[:if, [:vcall, :b], nil, [:vcall, :a]],
 "Ruby"=>"a if not b",
 "RubyParser"=>
  s(:if, s(:call, nil, :b, s(:arglist)), nil, s(:call, nil, :a, s(:arglist))),
 "Ruby2Ruby"=>"a unless b"}
end

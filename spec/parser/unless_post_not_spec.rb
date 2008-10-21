def test_case
{"RawParseTree"=>[:if, [:vcall, :b], [:vcall, :a], nil],
 "Ruby"=>"a unless not b",
 "RubyParser"=>
  s(:if, s(:call, nil, :b, s(:arglist)), s(:call, nil, :a, s(:arglist)), nil),
 "Ruby2Ruby"=>"a if b"}
end

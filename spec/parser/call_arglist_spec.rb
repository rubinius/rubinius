def test_case
{"RawParseTree"=>[:call, [:vcall, :o], :puts, [:array, [:lit, 42]]],
 "Ruby"=>"o.puts(42)",
 "RubyParser"=>
  s(:call, s(:call, nil, :o, s(:arglist)), :puts, s(:arglist, s(:lit, 42)))}
end

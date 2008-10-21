def test_case
{"RawParseTree"=>[:call, [:lit, 1], :b, [:array, [:vcall, :c]]],
 "Ruby"=>"1.b(c)",
 "RubyParser"=>
  s(:call, s(:lit, 1), :b, s(:arglist, s(:call, nil, :c, s(:arglist))))}
end

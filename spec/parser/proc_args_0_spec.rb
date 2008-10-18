def test_case
{"RawParseTree"=>
  [:iter, [:fcall, :proc], 0, [:call, [:vcall, :x], :+, [:array, [:lit, 1]]]],
 "Ruby"=>"proc { || (x + 1) }",
 "ParseTree"=>
  s(:iter,
   s(:call, nil, :proc, s(:arglist)),
   0,
   s(:call, s(:call, nil, :x, s(:arglist)), :+, s(:arglist, s(:lit, 1))))}
end

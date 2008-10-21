def test_case
{"RawParseTree"=>
  [:iter,
   [:fcall, :proc],
   nil,
   [:call, [:vcall, :x], :+, [:array, [:lit, 1]]]],
 "Ruby"=>"proc { (x + 1) }",
 "RubyParser"=>
  s(:iter,
   s(:call, nil, :proc, s(:arglist)),
   nil,
   s(:call, s(:call, nil, :x, s(:arglist)), :+, s(:arglist, s(:lit, 1))))}
end

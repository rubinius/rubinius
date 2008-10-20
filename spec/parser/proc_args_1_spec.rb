def test_case
{"RawParseTree"=>
  [:iter,
   [:fcall, :proc],
   [:lasgn, :x],
   [:call, [:lvar, :x], :+, [:array, [:lit, 1]]]],
 "Ruby"=>"proc { |x| (x + 1) }",
 "ParseTree"=>
  s(:iter,
   s(:call, nil, :proc, s(:arglist)),
   s(:lasgn, :x),
   s(:call, s(:lvar, :x), :+, s(:arglist, s(:lit, 1))))}
end

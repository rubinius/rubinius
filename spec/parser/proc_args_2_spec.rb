def test_case
{"RawParseTree"=>
  [:iter,
   [:fcall, :proc],
   [:masgn, [:array, [:dasgn_curr, :x], [:dasgn_curr, :y]], nil, nil],
   [:call, [:dvar, :x], :+, [:array, [:dvar, :y]]]],
 "Ruby"=>"proc { |x, y| (x + y) }",
 "ParseTree"=>
  s(:iter,
   s(:call, nil, :proc, s(:arglist)),
   s(:masgn, s(:array, s(:lasgn, :x), s(:lasgn, :y))),
   s(:call, s(:lvar, :x), :+, s(:arglist, s(:lvar, :y))))}
end

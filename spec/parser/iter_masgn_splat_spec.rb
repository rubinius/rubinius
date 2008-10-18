def test_case
{"RawParseTree"=>
  [:iter,
   [:fcall, :a],
   [:masgn, nil, [:dasgn_curr, :c], nil],
   [:fcall, :p, [:array, [:dvar, :c]]]],
 "Ruby"=>"a { |*c| p(c) }",
 "ParseTree"=>
  s(:iter,
   s(:call, nil, :a, s(:arglist)),
   s(:masgn, s(:array, s(:splat, s(:lasgn, :c)))),
   s(:call, nil, :p, s(:arglist, s(:lvar, :c))))}
end

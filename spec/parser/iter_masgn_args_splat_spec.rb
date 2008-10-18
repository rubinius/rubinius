def test_case
{"RawParseTree"=>
  [:iter,
   [:fcall, :a],
   [:masgn,
    [:array, [:dasgn_curr, :b], [:dasgn_curr, :c]],
    [:dasgn_curr, :d],
    nil],
   [:fcall, :p, [:array, [:dvar, :c]]]],
 "Ruby"=>"a { |b, c, *d| p(c) }",
 "ParseTree"=>
  s(:iter,
   s(:call, nil, :a, s(:arglist)),
   s(:masgn,
    s(:array, s(:lasgn, :b), s(:lasgn, :c), s(:splat, s(:lasgn, :d)))),
   s(:call, nil, :p, s(:arglist, s(:lvar, :c))))}
end

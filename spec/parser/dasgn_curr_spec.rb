def test_case
{"RawParseTree"=>
  [:iter,
   [:call, [:vcall, :data], :each],
   [:masgn, [:array, [:dasgn_curr, :x], [:dasgn_curr, :y]], nil, nil],
   [:block,
    [:dasgn_curr, :a, [:lit, 1]],
    [:dasgn_curr, :b, [:dvar, :a]],
    [:dasgn_curr, :b, [:dasgn_curr, :a, [:dvar, :x]]]]],
 "Ruby"=>"data.each do |x, y|\n  a = 1\n  b = a\n  b = a = x\nend",
 "ParseTree"=>
  s(:iter,
   s(:call, s(:call, nil, :data, s(:arglist)), :each, s(:arglist)),
   s(:masgn, s(:array, s(:lasgn, :x), s(:lasgn, :y))),
   s(:block,
    s(:lasgn, :a, s(:lit, 1)),
    s(:lasgn, :b, s(:lvar, :a)),
    s(:lasgn, :b, s(:lasgn, :a, s(:lvar, :x)))))}
end

def test_case
{"RawParseTree"=>
  [:iter,
   [:call, [:vcall, :data], :each],
   [:masgn, [:array, [:lasgn, :x], [:lasgn, :y]]],
   [:block,
    [:lasgn, :a, [:lit, 1]],
    [:lasgn, :b, [:lvar, :a]],
    [:lasgn, :b, [:lasgn, :a, [:lvar, :x]]]]],
 "Ruby"=>"data.each do |x, y|\n  a = 1\n  b = a\n  b = a = x\nend",
 "RubyParser"=>
  s(:iter,
   s(:call, s(:call, nil, :data, s(:arglist)), :each, s(:arglist)),
   s(:masgn, s(:array, s(:lasgn, :x), s(:lasgn, :y))),
   s(:block,
    s(:lasgn, :a, s(:lit, 1)),
    s(:lasgn, :b, s(:lvar, :a)),
    s(:lasgn, :b, s(:lasgn, :a, s(:lvar, :x)))))}
end

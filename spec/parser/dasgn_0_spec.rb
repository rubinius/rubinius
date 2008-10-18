def test_case
{"RawParseTree"=>
  [:iter,
   [:call, [:vcall, :a], :each],
   [:dasgn_curr, :x],
   [:if,
    [:true],
    [:iter,
     [:call, [:vcall, :b], :each],
     [:dasgn_curr, :y],
     [:dasgn, :x, [:call, [:dvar, :x], :+, [:array, [:lit, 1]]]]],
    nil]],
 "Ruby"=>"a.each { |x| b.each { |y| x = (x + 1) } if true }",
 "ParseTree"=>
  s(:iter,
   s(:call, s(:call, nil, :a, s(:arglist)), :each, s(:arglist)),
   s(:lasgn, :x),
   s(:if,
    s(:true),
    s(:iter,
     s(:call, s(:call, nil, :b, s(:arglist)), :each, s(:arglist)),
     s(:lasgn, :y),
     s(:lasgn, :x, s(:call, s(:lvar, :x), :+, s(:arglist, s(:lit, 1))))),
    nil))}
end

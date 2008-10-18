def test_case
{"RawParseTree"=>
  [:iter,
   [:call, [:vcall, :a], :each],
   [:dasgn_curr, :x],
   [:if,
    [:true],
    [:block,
     [:dasgn_curr, :c, [:lit, 0]],
     [:iter,
      [:call, [:vcall, :b], :each],
      [:dasgn_curr, :y],
      [:dasgn, :c, [:call, [:dvar, :c], :+, [:array, [:lit, 1]]]]]],
    nil]],
 "Ruby"=>
  "a.each do |x|\n  if true then\n    c = 0\n    b.each { |y| c = (c + 1) }\n  end\nend",
 "ParseTree"=>
  s(:iter,
   s(:call, s(:call, nil, :a, s(:arglist)), :each, s(:arglist)),
   s(:lasgn, :x),
   s(:if,
    s(:true),
    s(:block,
     s(:lasgn, :c, s(:lit, 0)),
     s(:iter,
      s(:call, s(:call, nil, :b, s(:arglist)), :each, s(:arglist)),
      s(:lasgn, :y),
      s(:lasgn, :c, s(:call, s(:lvar, :c), :+, s(:arglist, s(:lit, 1)))))),
    nil))}
end

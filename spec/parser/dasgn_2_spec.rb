def test_case
{"RawParseTree"=>
  [:iter,
   [:call, [:vcall, :a], :each],
   [:lasgn, :x],
   [:if,
    [:true],
    [:block,
     [:lasgn, :c, [:lit, 0]],
     [:iter,
      [:call, [:vcall, :b], :each],
      [:lasgn, :y],
      [:lasgn, :c, [:call, [:lvar, :c], :+, [:array, [:lit, 1]]]]]],
    nil]],
 "Ruby"=>
  "a.each do |x|\n  if true then\n    c = 0\n    b.each { |y| c = (c + 1) }\n  end\nend",
 "RubyParser"=>
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

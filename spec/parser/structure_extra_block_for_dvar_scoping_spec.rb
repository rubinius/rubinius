def test_case
{"RawParseTree"=>
  [:iter,
   [:call, [:vcall, :a], :b],
   [:masgn, [:array, [:dasgn_curr, :c], [:dasgn_curr, :d]], nil, nil],
   [:if,
    [:call, [:vcall, :e], :f, [:array, [:dvar, :c]]],
    nil,
    [:block,
     [:dasgn_curr, :g, [:false]],
     [:iter,
      [:call, [:dvar, :d], :h],
      [:masgn, [:array, [:dasgn_curr, :x], [:dasgn_curr, :i]], nil, nil],
      [:dasgn, :g, [:true]]]]]],
 "Ruby"=>
  "a.b do |c, d|\n  unless e.f(c) then\n    g = false\n    d.h { |x, i| g = true }\n  end\nend",
 "ParseTree"=>
  s(:iter,
   s(:call, s(:call, nil, :a, s(:arglist)), :b, s(:arglist)),
   s(:masgn, s(:array, s(:lasgn, :c), s(:lasgn, :d))),
   s(:if,
    s(:call, s(:call, nil, :e, s(:arglist)), :f, s(:arglist, s(:lvar, :c))),
    nil,
    s(:block,
     s(:lasgn, :g, s(:false)),
     s(:iter,
      s(:call, s(:lvar, :d), :h, s(:arglist)),
      s(:masgn, s(:array, s(:lasgn, :x), s(:lasgn, :i))),
      s(:lasgn, :g, s(:true))))))}
end

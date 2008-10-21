def test_case
{"RawParseTree"=>
  [:iter,
   [:call, [:vcall, :a], :b],
   [:masgn, [:array, [:lasgn, :c], [:lasgn, :d]]],
   [:if,
    [:call, [:vcall, :e], :f, [:array, [:lvar, :c]]],
    nil,
    [:block,
     [:lasgn, :g, [:false]],
     [:iter,
      [:call, [:lvar, :d], :h],
      [:masgn, [:array, [:lasgn, :x], [:lasgn, :i]]],
      [:lasgn, :g, [:true]]]]]],
 "Ruby"=>
  "a.b do |c, d|\n  unless e.f(c) then\n    g = false\n    d.h { |x, i| g = true }\n  end\nend",
 "RubyParser"=>
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

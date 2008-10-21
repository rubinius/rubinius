def test_case
{"RawParseTree"=>
  [:iter,
   [:fcall, :a, [:array, [:vcall, :b]]],
   nil,
   [:if,
    [:vcall, :b],
    [:true],
    [:block,
     [:lasgn, :c, [:false]],
     [:iter, [:fcall, :d], [:lasgn, :x], [:lasgn, :c, [:true]]],
     [:lvar, :c]]]],
 "Ruby"=>
  "a(b) do\n  if b then\n    true\n  else\n    c = false\n    d { |x| c = true }\n    c\n  end\nend",
 "RubyParser"=>
  s(:iter,
   s(:call, nil, :a, s(:arglist, s(:call, nil, :b, s(:arglist)))),
   nil,
   s(:if,
    s(:call, nil, :b, s(:arglist)),
    s(:true),
    s(:block,
     s(:lasgn, :c, s(:false)),
     s(:iter,
      s(:call, nil, :d, s(:arglist)),
      s(:lasgn, :x),
      s(:lasgn, :c, s(:true))),
     s(:lvar, :c))))}
end

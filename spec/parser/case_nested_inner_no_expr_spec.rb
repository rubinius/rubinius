def test_case
{"RawParseTree"=>
  [:case,
   [:vcall, :a],
   [:when,
    [:array, [:vcall, :b]],
    [:case,
     nil,
     [:when, [:array, [:and, [:vcall, :d], [:vcall, :e]]], [:vcall, :f]],
     nil]],
   nil],
 "Ruby"=>
  "case a\nwhen b then\n  case\n  when (d and e) then\n    f\n  else\n    # do nothing\n  end\nelse\n  # do nothing\nend",
 "RubyParser"=>
  s(:case,
   s(:call, nil, :a, s(:arglist)),
   s(:when,
    s(:array, s(:call, nil, :b, s(:arglist))),
    s(:case,
     nil,
     s(:when,
      s(:array,
       s(:and,
        s(:call, nil, :d, s(:arglist)),
        s(:call, nil, :e, s(:arglist)))),
      s(:call, nil, :f, s(:arglist))),
     nil)),
   nil)}
end

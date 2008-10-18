def test_case
{"RawParseTree"=>
  [:case,
   nil,
   [:when,
    [:array, [:call, [:vcall, :a], :==, [:array, [:lit, 1]]]],
    [:lit, :a]],
   [:when,
    [:array, [:call, [:vcall, :a], :==, [:array, [:lit, 2]]]],
    [:lit, :b]],
   [:lit, :c]],
 "Ruby"=>
  "case\nwhen (a == 1) then\n  :a\nwhen (a == 2) then\n  :b\nelse\n  :c\nend",
 "ParseTree"=>
  s(:case,
   nil,
   s(:when,
    s(:array,
     s(:call, s(:call, nil, :a, s(:arglist)), :==, s(:arglist, s(:lit, 1)))),
    s(:lit, :a)),
   s(:when,
    s(:array,
     s(:call, s(:call, nil, :a, s(:arglist)), :==, s(:arglist, s(:lit, 2)))),
    s(:lit, :b)),
   s(:lit, :c))}
end

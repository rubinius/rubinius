def test_case
{"RawParseTree"=>
  [:if,
   [:block,
    [:lasgn, :x, [:lit, 5]],
    [:call, [:lvar, :x], :+, [:array, [:lit, 1]]]],
   [:nil],
   nil],
 "Ruby"=>"if (x = 5\n(x + 1)) then\n  nil\nend",
 "RubyParser"=>
  s(:if,
   s(:block,
    s(:lasgn, :x, s(:lit, 5)),
    s(:call, s(:lvar, :x), :+, s(:arglist, s(:lit, 1)))),
   s(:nil),
   nil)}
end

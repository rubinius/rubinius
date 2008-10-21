def test_case
{"RawParseTree"=>
  [:lasgn,
   :x,
   [:block,
    [:lasgn, :y, [:lit, 1]],
    [:call, [:lvar, :y], :+, [:array, [:lit, 2]]]]],
 "Ruby"=>"x = (y = 1\n(y + 2))",
 "RubyParser"=>
  s(:lasgn,
   :x,
   s(:block,
    s(:lasgn, :y, s(:lit, 1)),
    s(:call, s(:lvar, :y), :+, s(:arglist, s(:lit, 2)))))}
end

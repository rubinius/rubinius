def test_case
{"RawParseTree"=>
  [:masgn,
   [:array, [:lasgn, :a], [:lasgn, :b]],
   [:lasgn, :c],
   [:argscat, [:array, [:lit, 1], [:lit, 2]], [:array, [:lit, 3], [:lit, 4]]]],
 "Ruby"=>"a, b, *c = 1, 2, *[3, 4]",
 "ParseTree"=>
  s(:masgn,
   s(:array, s(:lasgn, :a), s(:lasgn, :b), s(:splat, s(:lasgn, :c))),
   s(:array,
    s(:lit, 1),
    s(:lit, 2),
    s(:splat, s(:array, s(:lit, 3), s(:lit, 4)))))}
end

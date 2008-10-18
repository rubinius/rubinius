def test_case
{"RawParseTree"=>
  [:begin,
   [:ensure,
    [:rescue,
     [:call, [:lit, 1], :+, [:array, [:lit, 1]]],
     [:resbody,
      [:array, [:const, :SyntaxError]],
      [:block, [:lasgn, :e1, [:gvar, :$!]], [:lit, 2]],
      [:resbody,
       [:array, [:const, :Exception]],
       [:block, [:lasgn, :e2, [:gvar, :$!]], [:lit, 3]]]],
     [:lit, 4]],
    [:lit, 5]]],
 "Ruby"=>
  "begin\n  (1 + 1)\nrescue SyntaxError => e1\n  2\nrescue Exception => e2\n  3\nelse\n  4\nensure\n  5\nend",
 "ParseTree"=>
  s(:ensure,
   s(:rescue,
    s(:call, s(:lit, 1), :+, s(:arglist, s(:lit, 1))),
    s(:resbody,
     s(:array, s(:const, :SyntaxError), s(:lasgn, :e1, s(:gvar, :$!))),
     s(:lit, 2)),
    s(:resbody,
     s(:array, s(:const, :Exception), s(:lasgn, :e2, s(:gvar, :$!))),
     s(:lit, 3)),
    s(:lit, 4)),
   s(:lit, 5))}
end

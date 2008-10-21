def test_case
{"RawParseTree"=>
  [:begin,
   [:rescue,
    [:lit, 1],
    [:resbody,
     nil,
     [:block, [:lasgn, :e, [:gvar, :$!]], [:lasgn, :var, [:lit, 2]]]]]],
 "Ruby"=>"begin\n  1\nrescue => e\n  var = 2\nend",
 "RubyParser"=>
  s(:rescue,
   s(:lit, 1),
   s(:resbody,
    s(:array, s(:lasgn, :e, s(:gvar, :$!))),
    s(:lasgn, :var, s(:lit, 2))))}
end

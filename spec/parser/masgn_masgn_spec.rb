def test_case
{"RawParseTree"=>
  [:masgn,
   [:array,
    [:lasgn, :a],
    [:masgn, [:array, [:lasgn, :b], [:lasgn, :c]], nil, nil]],
   nil,
   [:to_ary, [:array, [:lit, 1], [:array, [:lit, 2], [:lit, 3]]]]],
 "Ruby"=>"a, (b, c) = [1, [2, 3]]",
 "ParseTree"=>
  s(:masgn,
   s(:array,
    s(:lasgn, :a),
    s(:masgn, s(:array, s(:lasgn, :b), s(:lasgn, :c)))),
   s(:to_ary, s(:array, s(:lit, 1), s(:array, s(:lit, 2), s(:lit, 3)))))}
end

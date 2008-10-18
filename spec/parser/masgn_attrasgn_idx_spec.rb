def test_case
{"RawParseTree"=>
  [:block,
   [:masgn,
    [:array, [:lasgn, :a], [:lasgn, :i], [:lasgn, :j]],
    nil,
    [:array, [:zarray], [:lit, 1], [:lit, 2]]],
   [:masgn,
    [:array,
     [:attrasgn, [:lvar, :a], :[]=, [:array, [:lvar, :i]]],
     [:attrasgn, [:lvar, :a], :[]=, [:array, [:lvar, :j]]]],
    nil,
    [:array,
     [:call, [:lvar, :a], :[], [:array, [:lvar, :j]]],
     [:call, [:lvar, :a], :[], [:array, [:lvar, :i]]]]]],
 "Ruby"=>"a, i, j = [], 1, 2\na[i], a[j] = a[j], a[i]\n",
 "ParseTree"=>
  s(:block,
   s(:masgn,
    s(:array, s(:lasgn, :a), s(:lasgn, :i), s(:lasgn, :j)),
    s(:array, s(:array), s(:lit, 1), s(:lit, 2))),
   s(:masgn,
    s(:array,
     s(:attrasgn, s(:lvar, :a), :[]=, s(:arglist, s(:lvar, :i))),
     s(:attrasgn, s(:lvar, :a), :[]=, s(:arglist, s(:lvar, :j)))),
    s(:array,
     s(:call, s(:lvar, :a), :[], s(:arglist, s(:lvar, :j))),
     s(:call, s(:lvar, :a), :[], s(:arglist, s(:lvar, :i))))))}
end

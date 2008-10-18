def test_case
{"RawParseTree"=>
  [:defn,
   :zarray,
   [:scope,
    [:block, [:args], [:lasgn, :a, [:zarray]], [:return, [:lvar, :a]]]]],
 "Ruby"=>"def zarray\n  a = []\n  return a\nend",
 "ParseTree"=>
  s(:defn,
   :zarray,
   s(:args),
   s(:scope, s(:block, s(:lasgn, :a, s(:array)), s(:return, s(:lvar, :a)))))}
end

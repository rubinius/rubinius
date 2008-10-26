def test_case
{"RawParseTree"=>
  [:masgn,
   [:array, [:lasgn, :a]],
   [:splat, [:array, [:array, [:array, [:lit, 1]]]]]],
 "Ruby"=>"a, = *[[[1]]]",
 "RubyParser"=>
  s(:masgn,
   s(:array, s(:lasgn, :a)),
   s(:splat, s(:array, s(:array, s(:array, s(:lit, 1))))))}
end

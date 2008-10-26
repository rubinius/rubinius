def test_case
{"RawParseTree"=>
  [:masgn, [:lasgn, :a], [:array, [:lit, 1], [:lit, 2], [:lit, 3]]],
 "Ruby"=>"*a = 1, 2, 3",
 "RubyParser"=>
  s(:masgn,
   s(:array, s(:splat, s(:lasgn, :a))),
   s(:array, s(:lit, 1), s(:lit, 2), s(:lit, 3)))}
end

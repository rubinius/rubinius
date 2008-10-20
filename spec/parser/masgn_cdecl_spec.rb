def test_case
{"RawParseTree"=>
  [:masgn,
   [:array, [:cdecl, :A], [:cdecl, :B], [:cdecl, :C]],
   [:array, [:lit, 1], [:lit, 2], [:lit, 3]]],
 "Ruby"=>"A, B, C = 1, 2, 3",
 "ParseTree"=>
  s(:masgn,
   s(:array, s(:cdecl, :A), s(:cdecl, :B), s(:cdecl, :C)),
   s(:array, s(:lit, 1), s(:lit, 2), s(:lit, 3)))}
end

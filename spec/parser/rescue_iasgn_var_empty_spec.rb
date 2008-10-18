def test_case
{"RawParseTree"=>
  [:begin, [:rescue, [:lit, 1], [:resbody, nil, [:iasgn, :@e, [:gvar, :$!]]]]],
 "Ruby"=>"begin\n  1\nrescue => @e\n  # do nothing\nend",
 "ParseTree"=>
  s(:rescue,
   s(:lit, 1),
   s(:resbody, s(:array, s(:iasgn, :@e, s(:gvar, :$!))), nil))}
end

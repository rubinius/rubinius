def test_case
{"RawParseTree"=>
  [:begin, [:rescue, [:lit, 1], [:resbody, nil, [:lasgn, :e, [:gvar, :$!]]]]],
 "Ruby"=>"begin\n  1\nrescue => e\n  # do nothing\nend",
 "RubyParser"=>
  s(:rescue,
   s(:lit, 1),
   s(:resbody, s(:array, s(:lasgn, :e, s(:gvar, :$!))), nil))}
end

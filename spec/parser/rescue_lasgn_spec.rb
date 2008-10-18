def test_case
{"RawParseTree"=>
  [:begin, [:rescue, [:lit, 1], [:resbody, nil, [:lasgn, :var, [:lit, 2]]]]],
 "Ruby"=>"begin\n  1\nrescue\n  var = 2\nend",
 "ParseTree"=>
  s(:rescue, s(:lit, 1), s(:resbody, s(:array), s(:lasgn, :var, s(:lit, 2)))),
 "Ruby2Ruby"=>"1 rescue var = 2"}
end

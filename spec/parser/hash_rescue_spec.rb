def test_case
{"RawParseTree"=>
  [:hash, [:lit, 1], [:rescue, [:lit, 2], [:resbody, nil, [:lit, 3]]]],
 "Ruby"=>"{ 1 => (2 rescue 3) }",
 "ParseTree"=>
  s(:hash,
   s(:lit, 1),
   s(:rescue, s(:lit, 2), s(:resbody, s(:array), s(:lit, 3))))}
end

def test_case
{"RawParseTree"=>
  [:fcall,
   :m,
   [:array,
    [:lit, 42],
    [:hash, [:lit, :a], [:lit, 1], [:lit, :b], [:lit, 2]]]],
 "Ruby"=>"m(42, :a => 1, :b => 2)",
 "ParseTree"=>
  s(:call,
   nil,
   :m,
   s(:arglist,
    s(:lit, 42),
    s(:hash, s(:lit, :a), s(:lit, 1), s(:lit, :b), s(:lit, 2))))}
end

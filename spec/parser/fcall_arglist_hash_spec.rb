def test_case
{"RawParseTree"=>
  [:fcall,
   :m,
   [:array, [:hash, [:lit, :a], [:lit, 1], [:lit, :b], [:lit, 2]]]],
 "Ruby"=>"m(:a => 1, :b => 2)",
 "RubyParser"=>
  s(:call,
   nil,
   :m,
   s(:arglist, s(:hash, s(:lit, :a), s(:lit, 1), s(:lit, :b), s(:lit, 2))))}
end

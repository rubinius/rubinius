def test_case
{"RawParseTree"=>
  [:block_pass,
   [:vcall, :b],
   [:fcall, :a, [:array, [:lit, 1], [:lit, 2], [:lit, 3]]]],
 "Ruby"=>"a(1, 2, 3, &b)",
 "RubyParser"=>
  s(:call,
   nil,
   :a,
   s(:arglist,
    s(:lit, 1),
    s(:lit, 2),
    s(:lit, 3),
    s(:block_pass, s(:call, nil, :b, s(:arglist)))))}
end

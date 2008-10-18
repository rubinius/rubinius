def test_case
{"RawParseTree"=>
  [:block_pass, [:vcall, :b], [:fcall, :a, [:array, [:lit, 4]]]],
 "Ruby"=>"a(4, &b)",
 "ParseTree"=>
  s(:call,
   nil,
   :a,
   s(:arglist, s(:lit, 4), s(:block_pass, s(:call, nil, :b, s(:arglist)))))}
end

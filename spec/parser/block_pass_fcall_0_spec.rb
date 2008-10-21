def test_case
{"RawParseTree"=>[:block_pass, [:vcall, :b], [:fcall, :a]],
 "Ruby"=>"a(&b)",
 "RubyParser"=>
  s(:call,
   nil,
   :a,
   s(:arglist, s(:block_pass, s(:call, nil, :b, s(:arglist)))))}
end

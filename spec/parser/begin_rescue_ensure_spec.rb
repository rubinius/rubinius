def test_case
{"RawParseTree"=>
  [:begin, [:ensure, [:rescue, [:vcall, :a], [:resbody, nil]], [:nil]]],
 "Ruby"=>"begin\n  a\nrescue\n  # do nothing\nensure\n  # do nothing\nend",
 "RubyParser"=>
  s(:ensure,
   s(:rescue, s(:call, nil, :a, s(:arglist)), s(:resbody, s(:array), nil)),
   s(:nil))}
end

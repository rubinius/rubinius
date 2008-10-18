def test_case
{"RawParseTree"=>[:begin, [:rescue, [:vcall, :blah], [:resbody, nil]]],
 "Ruby"=>"begin\n  blah\nrescue\n  # do nothing\nend",
 "ParseTree"=>
  s(:rescue, s(:call, nil, :blah, s(:arglist)), s(:resbody, s(:array), nil))}
end

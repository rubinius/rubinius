def test_case
{"RawParseTree"=>[:begin, [:ensure, [:rescue, [:resbody, nil]], [:nil]]],
 "Ruby"=>
  "begin\n  # do nothing\nrescue\n  # do nothing\nensure\n  # do nothing\nend",
 "ParseTree"=>s(:ensure, s(:rescue, s(:resbody, s(:array), nil)), s(:nil))}
end

def test_case
{"RawParseTree"=>[:if, [:fcall, :block_given?], [:lit, 42], nil],
 "Ruby"=>"42 if block_given?",
 "ParseTree"=>
  s(:if, s(:call, nil, :block_given?, s(:arglist)), s(:lit, 42), nil)}
end

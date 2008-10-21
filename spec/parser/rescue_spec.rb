def test_case
{"RawParseTree"=>[:rescue, [:vcall, :blah], [:resbody, nil, [:nil]]],
 "Ruby"=>"blah rescue nil",
 "RubyParser"=>
  s(:rescue,
   s(:call, nil, :blah, s(:arglist)),
   s(:resbody, s(:array), s(:nil)))}
end

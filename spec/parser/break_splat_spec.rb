def test_case
{"RawParseTree"=>
  [:iter, [:fcall, :loop], nil, [:break, [:svalue, [:splat, [:array, [:nil]]]]]],
 "Ruby"=>"loop { break *[nil] }",
 "RubyParser"=>
  s(:iter,
   s(:call, nil, :loop, s(:arglist)),
   nil,
   s(:break, s(:svalue, s(:splat, s(:array, s(:nil))))))}
end

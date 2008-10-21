def test_case
{"RawParseTree"=>
  [:iter, [:fcall, :loop], nil, [:if, [:true], [:break, [:lit, 42]], nil]],
 "Ruby"=>"loop { break 42 if true }",
 "RubyParser"=>
  s(:iter,
   s(:call, nil, :loop, s(:arglist)),
   nil,
   s(:if, s(:true), s(:break, s(:lit, 42)), nil))}
end

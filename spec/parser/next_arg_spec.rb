def test_case
{"RawParseTree"=>
  [:iter, [:fcall, :loop], nil, [:if, [:false], [:next, [:lit, 42]], nil]],
 "Ruby"=>"loop { next 42 if false }",
 "ParseTree"=>
  s(:iter,
   s(:call, nil, :loop, s(:arglist)),
   nil,
   s(:if, s(:false), s(:next, s(:lit, 42)), nil))}
end

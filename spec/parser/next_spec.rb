def test_case
{"RawParseTree"=>[:iter, [:fcall, :loop], nil, [:if, [:false], [:next], nil]],
 "Ruby"=>"loop { next if false }",
 "ParseTree"=>
  s(:iter,
   s(:call, nil, :loop, s(:arglist)),
   nil,
   s(:if, s(:false), s(:next), nil))}
end

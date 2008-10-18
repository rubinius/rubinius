def test_case
{"RawParseTree"=>[:iter, [:fcall, :loop], nil, [:if, [:true], [:break], nil]],
 "Ruby"=>"loop { break if true }",
 "ParseTree"=>
  s(:iter,
   s(:call, nil, :loop, s(:arglist)),
   nil,
   s(:if, s(:true), s(:break), nil))}
end

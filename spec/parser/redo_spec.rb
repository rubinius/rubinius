def test_case
{"RawParseTree"=>[:iter, [:fcall, :loop], nil, [:if, [:false], [:redo], nil]],
 "Ruby"=>"loop { redo if false }",
 "RubyParser"=>
  s(:iter,
   s(:call, nil, :loop, s(:arglist)),
   nil,
   s(:if, s(:false), s(:redo), nil))}
end

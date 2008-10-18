def test_case
{"RawParseTree"=>[:iter, [:fcall, :a, [:array, [:lit, :b]]], nil, [:lit, :c]],
 "Ruby"=>"a(:b) { :c }",
 "ParseTree"=>
  s(:iter, s(:call, nil, :a, s(:arglist, s(:lit, :b))), nil, s(:lit, :c))}
end

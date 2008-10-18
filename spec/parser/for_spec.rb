def test_case
{"RawParseTree"=>
  [:for, [:vcall, :ary], [:lasgn, :o], [:fcall, :puts, [:array, [:lvar, :o]]]],
 "Ruby"=>"for o in ary do\n  puts(o)\nend",
 "ParseTree"=>
  s(:for,
   s(:call, nil, :ary, s(:arglist)),
   s(:lasgn, :o),
   s(:call, nil, :puts, s(:arglist, s(:lvar, :o))))}
end

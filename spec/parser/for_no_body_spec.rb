def test_case
{"RawParseTree"=>[:for, [:dot2, [:lit, 0], [:vcall, :max]], [:lasgn, :i]],
 "Ruby"=>"for i in (0..max) do\n  # do nothing\nend",
 "ParseTree"=>
  s(:for,
   s(:dot2, s(:lit, 0), s(:call, nil, :max, s(:arglist))),
   s(:lasgn, :i))}
end

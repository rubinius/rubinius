def test_case
{"RawParseTree"=>
  [:block, [:lasgn, :t, [:lit, 5]], [:dxstr, "touch ", [:evstr, [:lvar, :t]]]],
 "Ruby"=>"t = 5\n`touch \#{t}`\n",
 "ParseTree"=>
  s(:block,
   s(:lasgn, :t, s(:lit, 5)),
   s(:dxstr, "touch ", s(:evstr, s(:lvar, :t))))}
end

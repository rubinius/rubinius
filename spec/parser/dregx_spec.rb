def test_case
{"RawParseTree"=>
  [:dregx,
   "x",
   [:evstr, [:call, [:lit, 1], :+, [:array, [:lit, 1]]]],
   [:str, "y"]],
 "Ruby"=>"/x\#{(1 + 1)}y/",
 "ParseTree"=>
  s(:dregx,
   "x",
   s(:evstr, s(:call, s(:lit, 1), :+, s(:arglist, s(:lit, 1)))),
   s(:str, "y"))}
end

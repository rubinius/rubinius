def test_case
{"RawParseTree"=>
  [:dregx_once,
   "x",
   [:evstr, [:call, [:lit, 1], :+, [:array, [:lit, 1]]]],
   [:str, "y"]],
 "Ruby"=>"/x\#{(1 + 1)}y/o",
 "RubyParser"=>
  s(:dregx_once,
   "x",
   s(:evstr, s(:call, s(:lit, 1), :+, s(:arglist, s(:lit, 1)))),
   s(:str, "y"))}
end

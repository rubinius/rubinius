def test_case
{"RawParseTree"=>
  [:dstr,
   "  blah\n",
   [:evstr, [:call, [:lit, 1], :+, [:array, [:lit, 1]]]],
   [:str, "blah\n"]],
 "Ruby"=>"<<EOM\n  blah\n\#{1 + 1}blah\nEOM\n",
 "ParseTree"=>
  s(:dstr,
   "  blah\n",
   s(:evstr, s(:call, s(:lit, 1), :+, s(:arglist, s(:lit, 1)))),
   s(:str, "blah\n")),
 "Ruby2Ruby"=>"\"  blah\\n\#{(1 + 1)}blah\\n\""}
end

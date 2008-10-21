def test_case
{"RawParseTree"=>
  [:block,
   [:lasgn, :argl, [:lit, 1]],
   [:dstr,
    "x",
    [:evstr, [:call, [:str, "%.2f"], :%, [:array, [:lit, 3.14159]]]],
    [:str, "y"]]],
 "Ruby"=>"argl = 1\n\"x\#{(\"%.2f\" % 3.14159)}y\"\n",
 "RubyParser"=>
  s(:block,
   s(:lasgn, :argl, s(:lit, 1)),
   s(:dstr,
    "x",
    s(:evstr, s(:call, s(:str, "%.2f"), :%, s(:arglist, s(:lit, 3.14159)))),
    s(:str, "y")))}
end

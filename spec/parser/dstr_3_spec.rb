def test_case
{"RawParseTree"=>
  [:block,
   [:lasgn, :max, [:lit, 2]],
   [:lasgn, :argl, [:lit, 1]],
   [:dstr,
    "x",
    [:evstr,
     [:call,
      [:dstr, "%.", [:evstr, [:lvar, :max]], [:str, "f"]],
      :%,
      [:array, [:lit, 3.14159]]]],
    [:str, "y"]]],
 "Ruby"=>"max = 2\nargl = 1\n\"x\#{(\"%.\#{max}f\" % 3.14159)}y\"\n",
 "ParseTree"=>
  s(:block,
   s(:lasgn, :max, s(:lit, 2)),
   s(:lasgn, :argl, s(:lit, 1)),
   s(:dstr,
    "x",
    s(:evstr,
     s(:call,
      s(:dstr, "%.", s(:evstr, s(:lvar, :max)), s(:str, "f")),
      :%,
      s(:arglist, s(:lit, 3.14159)))),
    s(:str, "y")))}
end

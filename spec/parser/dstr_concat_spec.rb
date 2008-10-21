def test_case
{"RawParseTree"=>
  [:dstr,
   "",
   [:evstr, [:lit, 22]],
   [:str, "aa"],
   [:str, "cd"],
   [:evstr, [:lit, 44]],
   [:str, "55"],
   [:evstr, [:lit, 66]]],
 "Ruby"=>"\"\#{22}aa\" \"cd\#{44}\" \"55\" \"\#{66}\"",
 "RubyParser"=>
  s(:dstr,
   "",
   s(:evstr, s(:lit, 22)),
   s(:str, "aa"),
   s(:str, "cd"),
   s(:evstr, s(:lit, 44)),
   s(:str, "55"),
   s(:evstr, s(:lit, 66))),
 "Ruby2Ruby"=>"\"\#{22}aacd\#{44}55\#{66}\""}
end

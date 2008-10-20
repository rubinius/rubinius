def test_case
{"RawParseTree"=>
  [:dstr, "blah",
   [:evstr, [:file]],
   [:str, ":"],
   [:evstr, [:lit, 1]],
   [:str, ": warning: "],
   [:evstr, [:call, [:gvar, :$!], :message]],
   [:str, " ("],
   [:evstr, [:call, [:gvar, :$!], :class]],
   [:str, ")"]],
 "Ruby"=>
  "\"\#{\"blah\"}\#{__FILE__}:\#{__LINE__}: warning: \#{$!.message} (\#{$!.class})\"",
 "ParseTree"=>
  s(:dstr,
   "blah(string):",
   s(:evstr, s(:lit, 1)),
   s(:str, ": warning: "),
   s(:evstr, s(:call, s(:gvar, :$!), :message, s(:arglist))),
   s(:str, " ("),
   s(:evstr, s(:call, s(:gvar, :$!), :class, s(:arglist))),
   s(:str, ")")),
 "Ruby2Ruby"=>"\"blah(string):\#{1}: warning: \#{$!.message} (\#{$!.class})\""}
end

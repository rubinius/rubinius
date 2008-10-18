def test_case
{"RawParseTree"=>
  [:dstr,
   "a ",
   [:evstr, [:gvar, :$global]],
   [:str, " b "],
   [:evstr, [:ivar, :@ivar]],
   [:str, " c "],
   [:evstr, [:cvar, :@@cvar]],
   [:str, " d"]],
 "Ruby"=>"\"a \#$global b \#@ivar c \#@@cvar d\"",
 "ParseTree"=>
  s(:dstr,
   "a ",
   s(:evstr, s(:gvar, :$global)),
   s(:str, " b "),
   s(:evstr, s(:ivar, :@ivar)),
   s(:str, " c "),
   s(:evstr, s(:cvar, :@@cvar)),
   s(:str, " d")),
 "Ruby2Ruby"=>"\"a \#{$global} b \#{@ivar} c \#{@@cvar} d\""}
end

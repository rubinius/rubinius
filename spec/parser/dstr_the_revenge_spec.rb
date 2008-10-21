def test_case
{"RawParseTree"=>
  [:dstr,
   "before ",
   [:evstr, [:vcall, :from]],
   [:str, " middle "],
   [:evstr, [:vcall, :to]],
   [:str, " ("],
   [:evstr, [:file]],
   [:str, ":"],
   [:evstr, [:lit, 1]],
   [:str, ")"]],
 "Ruby"=>"\"before \#{from} middle \#{to} (\#{__FILE__}:\#{__LINE__})\"",
 "RubyParser"=>
  s(:dstr,
   "before ",
   s(:evstr, s(:call, nil, :from, s(:arglist))),
   s(:str, " middle "),
   s(:evstr, s(:call, nil, :to, s(:arglist))),
   s(:str, " ("),
   s(:str, "(string)"),
   s(:str, ":"),
   s(:evstr, s(:lit, 1)),
   s(:str, ")")),
 "Ruby2Ruby"=>"\"before \#{from} middle \#{to} ((string):\#{1})\""}
end

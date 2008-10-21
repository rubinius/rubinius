def test_case
{"RawParseTree"=>
  [:dstr,
   "s1 '",
   [:evstr, [:const, :RUBY_PLATFORM]],
   [:str, "' s2\n"],
   [:evstr, [:file]],
   [:str, "\n"]],
 "Ruby"=>"<<-EOF\ns1 '\#{RUBY_PLATFORM}' s2\n\#{__FILE__}\n        EOF\n",
 "RubyParser"=>
  s(:dstr,
   "s1 '",
   s(:evstr, s(:const, :RUBY_PLATFORM)),
   s(:str, "' s2\n"),
   s(:str, "(string)"),
   s(:str, "\n")),
 "Ruby2Ruby"=>"\"s1 '\#{RUBY_PLATFORM}' s2\\n(string)\\n\""}
end

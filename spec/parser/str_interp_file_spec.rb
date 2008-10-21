def test_case
{"RawParseTree"=>[:dstr, "file = ", [:evstr, [:file]], [:str, "\n"]],
 "Ruby"=>"\"file = \#{__FILE__}\\n\"",
 "RubyParser"=>s(:str, "file = (string)\n"),
 "Ruby2Ruby"=>"\"file = (string)\\n\""}
end

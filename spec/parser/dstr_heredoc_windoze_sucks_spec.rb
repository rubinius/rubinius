def test_case
{"RawParseTree"=>
  [:dstr, "def test_", [:evstr, [:vcall, :action]], [:str, "_valid_feed\n"]],
 "Ruby"=>"<<-EOF\r\ndef test_\#{action}_valid_feed\r\n  EOF\r\n",
 "ParseTree"=>
  s(:dstr,
   "def test_",
   s(:evstr, s(:call, nil, :action, s(:arglist))),
   s(:str, "_valid_feed\n")),
 "Ruby2Ruby"=>"\"def test_\#{action}_valid_feed\\n\""}
end

def test_case
{"RawParseTree"=>[:regex, "x", 16],
 "Ruby"=>"/x/n",
 "RubyParser"=>s(:lit, /x/n),
 "SydneyParser"=>s(:regex, "x", 16),
 "Ruby2Ruby"=>"/x/n"}
end

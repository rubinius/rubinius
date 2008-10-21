def test_case
{"RawParseTree"=>[:regex, "x", 0],
 "Ruby"=>"/x/o",
 "RubyParser"=>s(:lit, /x/),
 "SydneyParser"=>s(:regex, "x", 0),
 "Ruby2Ruby"=>"/x/"}
end

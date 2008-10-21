def test_case
{"RawParseTree"=>[:dregx, "", [:evstr, [:lit, 1]], 16],
 "Ruby"=>"/\#{1}/n",
 "RubyParser"=>s(:dregx, "", s(:evstr, s(:lit, 1)), 16),
 "Ruby2Ruby"=>"/\#{1}/"}
end

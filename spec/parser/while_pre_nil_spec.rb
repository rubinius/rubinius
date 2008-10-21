def test_case
{"RawParseTree"=>[:while, [:false], nil, true],
 "Ruby"=>"while false do\nend",
 "RubyParser"=>s(:while, s(:false), nil, true)}
end

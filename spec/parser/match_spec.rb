def test_case
{"RawParseTree"=>[:if, [:match, "x", 0], [:lit, 1], nil],
 "Ruby"=>"1 if /x/",
 "RubyParser"=>s(:if, s(:match, s(:lit, /x/)), s(:lit, 1), nil),
 "SydneyParser"=>s(:if, s(:match, s(:regex, "x", 0)), s(:lit, 1), nil)}
end

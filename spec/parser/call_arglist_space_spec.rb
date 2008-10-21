def test_case
{"RawParseTree"=>[:fcall, :a, [:array, [:lit, 1], [:lit, 2], [:lit, 3]]],
 "Ruby"=>"a (1,2,3)",
 "RubyParser"=>
  s(:call, nil, :a, s(:arglist, s(:lit, 1), s(:lit, 2), s(:lit, 3))),
 "Ruby2Ruby"=>"a(1, 2, 3)"}
end

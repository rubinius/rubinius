def test_case
{"RawParseTree"=>[:fcall, :m, [:array, [:lit, 42]]],
 "Ruby"=>"m(42)",
 "RubyParser"=>s(:call, nil, :m, s(:arglist, s(:lit, 42)))}
end

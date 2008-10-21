def test_case
{"RawParseTree"=>[:fcall, :a, [:array, [:array, [:lit, 42]]]],
 "Ruby"=>"a [42]",
 "RubyParser"=>s(:call, nil, :a, s(:arglist, s(:array, s(:lit, 42)))),
 "Ruby2Ruby"=>"a([42])"}
end

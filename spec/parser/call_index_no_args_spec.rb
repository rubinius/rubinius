def test_case
{"RawParseTree"=>[:call, [:vcall, :a], :[]],
 "Ruby"=>"a[]",
 "RubyParser"=>s(:call, s(:call, nil, :a, s(:arglist)), :[], s(:arglist))}
end

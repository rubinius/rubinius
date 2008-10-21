def test_case
{"RawParseTree"=>[:yield, [:splat, [:vcall, :ary]], true],
 "Ruby"=>"yield(*ary)",
 "RubyParser"=>s(:yield, s(:splat, s(:call, nil, :ary, s(:arglist))))}
end

def test_case
{"RawParseTree"=>[:block, [:vcall, :f1], [:undef, [:lit, :x]]],
 "Ruby"=>"f1\nundef :x\n",
 "RubyParser"=>
  s(:block, s(:call, nil, :f1, s(:arglist)), s(:undef, s(:lit, :x)))}
end

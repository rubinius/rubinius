def test_case
{"RawParseTree"=>[:iter, [:fcall, :loop], nil],
 "Ruby"=>"loop { }",
 "RubyParser"=>s(:iter, s(:call, nil, :loop, s(:arglist)), nil)}
end

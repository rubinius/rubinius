def test_case
{"RawParseTree"=>[:class, :X, [:vcall, :expr], [:scope]],
 "Ruby"=>"class X < expr\nend",
 "RubyParser"=>s(:class, :X, s(:call, nil, :expr, s(:arglist)), s(:scope))}
end

def test_case
{"RawParseTree"=>[:class, :X, [:vcall, :expr], [:scope]],
 "Ruby"=>"class X < expr\nend",
 "ParseTree"=>s(:class, :X, s(:call, nil, :expr, s(:arglist)), s(:scope))}
end

def test_case
{"RawParseTree"=>
  [:class, [:colon2, [:const, :X], :Y], nil, [:scope, [:vcall, :c]]],
 "Ruby"=>"class X::Y\n  c\nend",
 "ParseTree"=>
  s(:class,
   s(:colon2, s(:const, :X), :Y),
   nil,
   s(:scope, s(:call, nil, :c, s(:arglist))))}
end

def test_case
{"RawParseTree"=>
  [:defs, [:call, [:vcall, :a], :b], :empty, [:scope, [:args, :*]]],
 "Ruby"=>"def (a.b).empty(*)\n  # do nothing\nend",
 "ParseTree"=>
  s(:defs,
   s(:call, s(:call, nil, :a, s(:arglist)), :b, s(:arglist)),
   :empty,
   s(:args, :*),
   s(:scope, s(:block)))}
end

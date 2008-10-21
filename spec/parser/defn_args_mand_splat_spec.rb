def test_case
{"RawParseTree"=>
  [:defn, :f, [:scope, [:block, [:args, :mand, :"*rest"], [:nil]]]],
 "Ruby"=>"def f(mand, *rest)\n  # do nothing\nend",
 "RubyParser"=>
  s(:defn, :f, s(:args, :mand, :"*rest"), s(:scope, s(:block, s(:nil))))}
end

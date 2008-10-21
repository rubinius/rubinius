def test_case
{"RawParseTree"=>[:defn, :f, [:scope, [:block, [:args, :"*rest"], [:nil]]]],
 "Ruby"=>"def f(*rest)\n  # do nothing\nend",
 "RubyParser"=>s(:defn, :f, s(:args, :"*rest"), s(:scope, s(:block, s(:nil))))}
end

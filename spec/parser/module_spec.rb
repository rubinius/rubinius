def test_case
{"RawParseTree"=>
  [:module, :X, [:scope, [:defn, :y, [:scope, [:block, [:args], [:nil]]]]]],
 "Ruby"=>"module X\n  def y\n    # do nothing\n  end\nend",
 "RubyParser"=>
  s(:module,
   :X,
   s(:scope, s(:defn, :y, s(:args), s(:scope, s(:block, s(:nil))))))}
end

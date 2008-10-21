def test_case
{"RawParseTree"=>
  [:block,
   [:lasgn, :y, [:lit, 0]],
   [:attrasgn, [:lit, 42], :method=, [:array, [:lvar, :y]]]],
 "Ruby"=>"y = 0\n42.method = y\n",
 "RubyParser"=>
  s(:block,
   s(:lasgn, :y, s(:lit, 0)),
   s(:attrasgn, s(:lit, 42), :method=, s(:arglist, s(:lvar, :y))))}
end

def test_case
{"RawParseTree"=>
  [:while,
   [:false],
   [:block,
    [:call, [:lit, 1], :+, [:array, [:lit, 2]]],
    [:call, [:lit, 3], :+, [:array, [:lit, 4]]]],
   false],
 "Ruby"=>"begin\n  (1 + 2)\n  (3 + 4)\nend while false",
 "RubyParser"=>
  s(:while,
   s(:false),
   s(:block,
    s(:call, s(:lit, 1), :+, s(:arglist, s(:lit, 2))),
    s(:call, s(:lit, 3), :+, s(:arglist, s(:lit, 4)))),
   false)}
end

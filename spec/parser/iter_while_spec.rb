def test_case
{"RawParseTree"=>
  [:block,
   [:lasgn, :argl, [:lit, 10]],
   [:while,
    [:call, [:lvar, :argl], :>=, [:array, [:lit, 1]]],
    [:block,
     [:fcall, :puts, [:array, [:str, "hello"]]],
     [:lasgn, :argl, [:call, [:lvar, :argl], :-, [:array, [:lit, 1]]]]],
    true]],
 "Ruby"=>
  "argl = 10\nwhile (argl >= 1) do\n  puts(\"hello\")\n  argl = (argl - 1)\nend\n",
 "ParseTree"=>
  s(:block,
   s(:lasgn, :argl, s(:lit, 10)),
   s(:while,
    s(:call, s(:lvar, :argl), :>=, s(:arglist, s(:lit, 1))),
    s(:block,
     s(:call, nil, :puts, s(:arglist, s(:str, "hello"))),
     s(:lasgn, :argl, s(:call, s(:lvar, :argl), :-, s(:arglist, s(:lit, 1))))),
    true))}
end

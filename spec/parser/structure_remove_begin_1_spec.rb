def test_case
{"RawParseTree"=>
  [:call,
   [:vcall, :a],
   :<<,
   [:array, [:rescue, [:vcall, :b], [:resbody, nil, [:vcall, :c]]]]],
 "Ruby"=>"a << begin\n       b\n     rescue\n       c\n     end",
 "ParseTree"=>
  s(:call,
   s(:call, nil, :a, s(:arglist)),
   :<<,
   s(:arglist,
    s(:rescue,
     s(:call, nil, :b, s(:arglist)),
     s(:resbody, s(:array), s(:call, nil, :c, s(:arglist)))))),
 "Ruby2Ruby"=>"(a << b rescue c)"}
end

def test_case
{"RawParseTree"=>
  [:begin,
   [:rescue,
    [:vcall, :a],
    [:resbody,
     nil,
     [:block, [:iasgn, :@e, [:gvar, :$!]], [:vcall, :c], [:vcall, :d]]]]],
 "Ruby"=>"begin\n  a\nrescue => @e\n  c\n  d\nend",
 "ParseTree"=>
  s(:rescue,
   s(:call, nil, :a, s(:arglist)),
   s(:resbody,
    s(:array, s(:iasgn, :@e, s(:gvar, :$!))),
    s(:block,
     s(:call, nil, :c, s(:arglist)),
     s(:call, nil, :d, s(:arglist)))))}
end

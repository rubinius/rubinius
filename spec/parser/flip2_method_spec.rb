def test_case
{"RawParseTree"=>
  [:if,
   [:flip2, [:lit, 1], [:call, [:lit, 2], :a?, [:array, [:vcall, :b]]]],
   [:nil],
   nil],
 "Ruby"=>"if 1..2.a?(b) then\n  nil\nend",
 "ParseTree"=>
  s(:if,
   s(:flip2,
    s(:lit, 1),
    s(:call, s(:lit, 2), :a?, s(:arglist, s(:call, nil, :b, s(:arglist))))),
   s(:nil),
   nil)}
end

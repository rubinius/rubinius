def test_case
{"RawParseTree"=>
  [:iter,
   [:fcall, :a],
   [:masgn, [:splat]],
   [:fcall, :p, [:array, [:vcall, :c]]]],
 "Ruby"=>"a { |*| p(c) }",
 "ParseTree"=>
  s(:iter,
   s(:call, nil, :a, s(:arglist)),
   s(:masgn, s(:array, s(:splat))),
   s(:call, nil, :p, s(:arglist, s(:call, nil, :c, s(:arglist)))))}
end

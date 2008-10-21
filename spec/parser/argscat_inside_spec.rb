def test_case
{"RawParseTree"=>
  [:lasgn, :a, [:argscat, [:array, [:vcall, :b]], [:vcall, :c]]],
 "Ruby"=>"a = [b, *c]",
 "RubyParser"=>
  s(:lasgn,
   :a,
   s(:array,
    s(:call, nil, :b, s(:arglist)),
    s(:splat, s(:call, nil, :c, s(:arglist)))))}
end

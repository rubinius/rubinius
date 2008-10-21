def test_case
{"RawParseTree"=>
  [:lasgn,
   :a,
   [:svalue, [:argscat, [:array, [:vcall, :b], [:vcall, :c]], [:vcall, :d]]]],
 "Ruby"=>"a = b, c, *d",
 "RubyParser"=>
  s(:lasgn,
   :a,
   s(:svalue,
    s(:array,
     s(:call, nil, :b, s(:arglist)),
     s(:call, nil, :c, s(:arglist)),
     s(:splat, s(:call, nil, :d, s(:arglist))))))}
end

def test_case
{"RawParseTree"=>
  [:if,
   [:lasgn, :x, [:call, [:vcall, :obj], :x]],
   [:call, [:lvar, :x], :do_it],
   nil],
 "Ruby"=>"if x = obj.x then\n  x.do_it\nend",
 "RubyParser"=>
  s(:if,
   s(:lasgn, :x, s(:call, s(:call, nil, :obj, s(:arglist)), :x, s(:arglist))),
   s(:call, s(:lvar, :x), :do_it, s(:arglist)),
   nil)}
end

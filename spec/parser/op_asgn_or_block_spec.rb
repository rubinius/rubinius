def test_case
{"RawParseTree"=>
  [:op_asgn_or,
   [:lvar, :a],
   [:lasgn, :a, [:rescue, [:vcall, :b], [:resbody, nil, [:vcall, :c]]]]],
 "Ruby"=>"a ||= begin\n        b\n      rescue\n        c\n      end",
 "RubyParser"=>
  s(:op_asgn_or,
   s(:lvar, :a),
   s(:lasgn,
    :a,
    s(:rescue,
     s(:call, nil, :b, s(:arglist)),
     s(:resbody, s(:array), s(:call, nil, :c, s(:arglist)))))),
 "Ruby2Ruby"=>"a ||= b rescue c"}
end

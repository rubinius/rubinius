def test_case
{"RawParseTree"=>
  [:iter,
   [:call, [:vcall, :as], :each],
   [:dasgn_curr, :a],
   [:dasgn_curr,
    :b,
    [:call,
     [:dvar, :b],
     :+,
     [:array, [:call, [:dvar, :a], :b, [:array, [:false]]]]]]],
 "Ruby"=>"as.each { |a|\n  b += a.b(false) }",
 "ParseTree"=>
  s(:iter,
   s(:call, s(:call, nil, :as, s(:arglist)), :each, s(:arglist)),
   s(:lasgn, :a),
   s(:lasgn,
    :b,
    s(:call,
     s(:lvar, :b),
     :+,
     s(:arglist, s(:call, s(:lvar, :a), :b, s(:arglist, s(:false))))))),
 "Ruby2Ruby"=>"as.each { |a| b = (b + a.b(false)) }"}
end

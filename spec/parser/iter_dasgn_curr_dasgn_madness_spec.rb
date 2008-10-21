def test_case
{"RawParseTree"=>
  [:iter,
   [:call, [:vcall, :as], :each],
   [:lasgn, :a],
   [:lasgn,
    :b,
    [:call,
     [:lvar, :b],
     :+,
     [:array, [:call, [:lvar, :a], :b, [:array, [:false]]]]]]],
 "Ruby"=>"as.each { |a|\n  b += a.b(false) }",
 "RubyParser"=>
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

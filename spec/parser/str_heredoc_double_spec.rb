def test_case
{"RawParseTree"=>
  [:lasgn,
   :a,
   [:call,
    [:lvar, :a],
    :+,
    [:array,
     [:call,
      [:call, [:str, "  first\n"], :+, [:array, [:vcall, :b]]],
      :+,
      [:array, [:str, "  second\n"]]]]]],
 "Ruby"=>"a += <<-H1 + b + <<-H2\n  first\nH1\n  second\nH2",
 "RubyParser"=>
  s(:lasgn,
   :a,
   s(:call,
    s(:lvar, :a),
    :+,
    s(:arglist,
     s(:call,
      s(:call,
       s(:str, "  first\n"),
       :+,
       s(:arglist, s(:call, nil, :b, s(:arglist)))),
      :+,
      s(:arglist, s(:str, "  second\n")))))),
 "Ruby2Ruby"=>"a = (a + ((\"  first\\n\" + b) + \"  second\\n\"))"}
end

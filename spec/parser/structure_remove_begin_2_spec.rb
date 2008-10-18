def test_case
{"RawParseTree"=>
  [:block,
   [:lasgn,
    :a,
    [:if,
     [:vcall, :c],
     [:rescue, [:vcall, :b], [:resbody, nil, [:nil]]],
     nil]],
   [:lvar, :a]],
 "Ruby"=>
  "a = if c\n      begin\n        b\n      rescue\n        nil\n      end\n    end\na",
 "ParseTree"=>
  s(:block,
   s(:lasgn,
    :a,
    s(:if,
     s(:call, nil, :c, s(:arglist)),
     s(:rescue,
      s(:call, nil, :b, s(:arglist)),
      s(:resbody, s(:array), s(:nil))),
     nil)),
   s(:lvar, :a)),
 "Ruby2Ruby"=>"a = b rescue nil if c\na\n"}
end

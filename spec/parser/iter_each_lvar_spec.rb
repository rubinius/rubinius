def test_case
{"RawParseTree"=>
  [:block,
   [:lasgn, :array, [:array, [:lit, 1], [:lit, 2], [:lit, 3]]],
   [:iter,
    [:call, [:lvar, :array], :each],
    [:dasgn_curr, :x],
    [:fcall, :puts, [:array, [:call, [:dvar, :x], :to_s]]]]],
 "Ruby"=>"array = [1, 2, 3]\narray.each { |x| puts(x.to_s) }\n",
 "ParseTree"=>
  s(:block,
   s(:lasgn, :array, s(:array, s(:lit, 1), s(:lit, 2), s(:lit, 3))),
   s(:iter,
    s(:call, s(:lvar, :array), :each, s(:arglist)),
    s(:lasgn, :x),
    s(:call,
     nil,
     :puts,
     s(:arglist, s(:call, s(:lvar, :x), :to_s, s(:arglist))))))}
end

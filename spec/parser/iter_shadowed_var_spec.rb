def test_case
{"RawParseTree"=>
  [:iter,
   [:fcall, :a],
   [:dasgn_curr, :x],
   [:iter,
    [:fcall, :b],
    [:dasgn, :x],
    [:fcall, :puts, [:array, [:dvar, :x]]]]],
 "Ruby"=>"a do |x|\n  b do |x|\n    puts x\n  end\nend",
 "ParseTree"=>
  s(:iter,
   s(:call, nil, :a, s(:arglist)),
   s(:lasgn, :x),
   s(:iter,
    s(:call, nil, :b, s(:arglist)),
    s(:lasgn, :x),
    s(:call, nil, :puts, s(:arglist, s(:lvar, :x))))),
 "Ruby2Ruby"=>"a { |x| b { |x| puts(x) } }"}
end

def test_case
{"RawParseTree"=>
  [:iter,
   [:call, [:lit, 1], :upto, [:array, [:lit, 3]]],
   [:lasgn, :n],
   [:fcall, :puts, [:array, [:call, [:lvar, :n], :to_s]]]],
 "Ruby"=>"1.upto(3) { |n| puts(n.to_s) }",
 "RubyParser"=>
  s(:iter,
   s(:call, s(:lit, 1), :upto, s(:arglist, s(:lit, 3))),
   s(:lasgn, :n),
   s(:call,
    nil,
    :puts,
    s(:arglist, s(:call, s(:lvar, :n), :to_s, s(:arglist)))))}
end

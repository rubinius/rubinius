def test_case
{"RawParseTree"=>
  [:iter,
   [:call, [:lit, 3], :downto, [:array, [:lit, 1]]],
   [:dasgn_curr, :n],
   [:fcall, :puts, [:array, [:call, [:dvar, :n], :to_s]]]],
 "Ruby"=>"3.downto(1) { |n| puts(n.to_s) }",
 "ParseTree"=>
  s(:iter,
   s(:call, s(:lit, 3), :downto, s(:arglist, s(:lit, 1))),
   s(:lasgn, :n),
   s(:call,
    nil,
    :puts,
    s(:arglist, s(:call, s(:lvar, :n), :to_s, s(:arglist)))))}
end

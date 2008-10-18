def test_case
{"RawParseTree"=>
  [:block_pass,
   [:iter,
    [:call, [:const, :Proc], :new],
    [:masgn, nil, [:dasgn_curr, :args], nil],
    [:nil]],
   [:fcall,
    :define_attr_method,
    [:array, [:lit, :x], [:lit, :sequence_name]]]],
 "Ruby"=>"define_attr_method(:x, :sequence_name, &Proc.new { |*args| nil })",
 "ParseTree"=>
  s(:call,
   nil,
   :define_attr_method,
   s(:arglist,
    s(:lit, :x),
    s(:lit, :sequence_name),
    s(:block_pass,
     s(:iter,
      s(:call, s(:const, :Proc), :new, s(:arglist)),
      s(:masgn, s(:array, s(:splat, s(:lasgn, :args)))),
      s(:nil)))))}
end

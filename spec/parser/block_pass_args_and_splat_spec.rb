def test_case
{"RawParseTree"=>
  [:defn,
   :blah,
   [:scope,
    [:block,
     [:args, :"*args"],
     [:block_arg, :block],
     [:block_pass,
      [:lvar, :block],
      [:fcall, :other, [:argscat, [:array, [:lit, 42]], [:lvar, :args]]]]]]],
 "Ruby"=>"def blah(*args, &block)\n  other(42, *args, &block)\nend",
 "RubyParser"=>
  s(:defn,
   :blah,
   s(:args, :"*args", :"&block"),
   s(:scope,
    s(:block,
     s(:call,
      nil,
      :other,
      s(:arglist,
       s(:lit, 42),
       s(:splat, s(:lvar, :args)),
       s(:block_pass, s(:lvar, :block)))))))}
end

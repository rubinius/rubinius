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
      [:fcall, :other, [:splat, [:lvar, :args]]]]]]],
 "Ruby"=>"def blah(*args, &block)\n  other(*args, &block)\nend",
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
       s(:splat, s(:lvar, :args)),
       s(:block_pass, s(:lvar, :block)))))))}
end

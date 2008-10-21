def test_case
{"RawParseTree"=>
  [:class,
   :X,
   nil,
   [:scope,
    [:block,
     [:fcall, :puts, [:array, [:call, [:lit, 1], :+, [:array, [:lit, 1]]]]],
     [:defn,
      :blah,
      [:scope,
       [:block, [:args], [:fcall, :puts, [:array, [:str, "hello"]]]]]]]]],
 "Ruby"=>
  "class X\n  puts((1 + 1))\n  def blah\n    puts(\"hello\")\n  end\nend",
 "RubyParser"=>
  s(:class,
   :X,
   nil,
   s(:scope,
    s(:block,
     s(:call,
      nil,
      :puts,
      s(:arglist, s(:call, s(:lit, 1), :+, s(:arglist, s(:lit, 1))))),
     s(:defn,
      :blah,
      s(:args),
      s(:scope,
       s(:block, s(:call, nil, :puts, s(:arglist, s(:str, "hello")))))))))}
end

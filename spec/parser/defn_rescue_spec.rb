def test_case
{"RawParseTree"=>
  [:defn,
   :eql?,
   [:scope,
    [:block,
     [:args, :resource],
     [:rescue,
      [:call,
       [:call, [:self], :uuid],
       :==,
       [:array, [:call, [:lvar, :resource], :uuid]]],
      [:resbody, nil, [:false]]]]]],
 "Ruby"=>
  "def eql?(resource)\n  (self.uuid == resource.uuid)\nrescue\n  false\nend",
 "RubyParser"=>
  s(:defn,
   :eql?,
   s(:args, :resource),
   s(:scope,
    s(:block,
     s(:rescue,
      s(:call,
       s(:call, s(:self), :uuid, s(:arglist)),
       :==,
       s(:arglist, s(:call, s(:lvar, :resource), :uuid, s(:arglist)))),
      s(:resbody, s(:array), s(:false)))))),
 "Ruby2Ruby"=>
  "def eql?(resource)\n  (self.uuid == resource.uuid) rescue false\nend"}
end

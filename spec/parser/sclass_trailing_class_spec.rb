def test_case
{"RawParseTree"=>
  [:class,
   :A,
   nil,
   [:scope,
    [:block,
     [:sclass, [:self], [:scope, [:vcall, :a]]],
     [:class, :B, nil, [:scope]]]]],
 "Ruby"=>"class A\n  class << self\n    a\n  end\n  class B\n  end\nend",
 "RubyParser"=>
  s(:class,
   :A,
   nil,
   s(:scope,
    s(:block,
     s(:sclass, s(:self), s(:scope, s(:call, nil, :a, s(:arglist)))),
     s(:class, :B, nil, s(:scope)))))}
end

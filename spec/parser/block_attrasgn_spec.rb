def test_case
{"RawParseTree"=>
  [:defs,
   [:self],
   :setup,
   [:scope,
    [:block,
     [:args, :ctx],
     [:lasgn, :bind, [:vcall, :allocate]],
     [:attrasgn, [:lvar, :bind], :context=, [:array, [:lvar, :ctx]]],
     [:return, [:lvar, :bind]]]]],
 "Ruby"=>
  "def self.setup(ctx)\n  bind = allocate\n  bind.context = ctx\n  return bind\nend",
 "ParseTree"=>
  s(:defs,
   s(:self),
   :setup,
   s(:args, :ctx),
   s(:scope,
    s(:block,
     s(:lasgn, :bind, s(:call, nil, :allocate, s(:arglist))),
     s(:attrasgn, s(:lvar, :bind), :context=, s(:arglist, s(:lvar, :ctx))),
     s(:return, s(:lvar, :bind)))))}
end

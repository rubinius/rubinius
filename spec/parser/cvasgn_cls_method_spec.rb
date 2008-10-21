def test_case
{"RawParseTree"=>
  [:defs,
   [:self],
   :quiet_mode=,
   [:scope,
    [:block, [:args, :boolean], [:cvasgn, :@@quiet_mode, [:lvar, :boolean]]]]],
 "Ruby"=>"def self.quiet_mode=(boolean)\n  @@quiet_mode = boolean\nend",
 "RubyParser"=>
  s(:defs,
   s(:self),
   :quiet_mode=,
   s(:args, :boolean),
   s(:scope, s(:block, s(:cvasgn, :@@quiet_mode, s(:lvar, :boolean)))))}
end

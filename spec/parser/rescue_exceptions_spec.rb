def test_case
{"RawParseTree"=>
  [:begin,
   [:rescue,
    [:vcall, :blah],
    [:resbody,
     [:array, [:const, :RuntimeError]],
     [:lasgn, :r, [:gvar, :$!]]]]],
 "Ruby"=>"begin\n  blah\nrescue RuntimeError => r\n  # do nothing\nend",
 "RubyParser"=>
  s(:rescue,
   s(:call, nil, :blah, s(:arglist)),
   s(:resbody,
    s(:array, s(:const, :RuntimeError), s(:lasgn, :r, s(:gvar, :$!))),
    nil))}
end

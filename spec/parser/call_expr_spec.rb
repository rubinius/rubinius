def test_case
{"RawParseTree"=>
  [:call, [:lasgn, :v, [:call, [:lit, 1], :+, [:array, [:lit, 1]]]], :zero?],
 "Ruby"=>"(v = (1 + 1)).zero?",
 "ParseTree"=>
  s(:call,
   s(:lasgn, :v, s(:call, s(:lit, 1), :+, s(:arglist, s(:lit, 1)))),
   :zero?,
   s(:arglist))}
end

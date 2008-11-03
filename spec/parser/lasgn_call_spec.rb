def test_case
{"RawParseTree"=>[:lasgn, :c, [:call, [:lit, 2], :+, [:array, [:lit, 3]]]],
 "Ruby"=>"c = (2 + 3)",
 "RubyParser"=>
  s(:lasgn, :c, s(:call, s(:lit, 2), :+, s(:arglist, s(:lit, 3))))}
end

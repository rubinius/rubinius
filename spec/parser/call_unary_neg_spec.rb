def test_case
{"RawParseTree"=>[:call, [:call, [:lit, 2], :**, [:array, [:lit, 31]]], :-@],
 "Ruby"=>"-2**31",
 "RubyParser"=>
  s(:call,
   s(:call, s(:lit, 2), :**, s(:arglist, s(:lit, 31))),
   :-@,
   s(:arglist)),
 "Ruby2Ruby"=>"-(2 ** 31)"}
end

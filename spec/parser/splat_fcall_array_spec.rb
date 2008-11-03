def test_case
{"RawParseTree"=>[:fcall, :meth, [:array, [:splat, [:array, [:lit, 1]]]]],
 "Ruby"=>"meth([*[1]])",
 "RubyParser"=>
  s(:call,
   nil,
   :meth,
   s(:arglist, s(:array, s(:splat, s(:array, s(:lit, 1))))))}
end

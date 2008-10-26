def test_case
{"RawParseTree"=>[:gasgn, :$a, [:svalue, [:splat, [:array, [:lit, 1]]]]],
 "Ruby"=>"$a = *[1]",
 "RubyParser"=>s(:gasgn, :$a, s(:svalue, s(:splat, s(:array, s(:lit, 1)))))}
end

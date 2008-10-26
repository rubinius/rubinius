def test_case
{"RawParseTree"=>[:iasgn, :@a, [:svalue, [:splat, [:array, [:lit, 1]]]]],
 "Ruby"=>"@a = *[1]",
 "RubyParser"=>s(:iasgn, :@a, s(:svalue, s(:splat, s(:array, s(:lit, 1)))))}
end

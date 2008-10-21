def test_case
{"RawParseTree"=>[:op_asgn_or, [:ivar, :@v], [:iasgn, :@v, [:hash]]],
 "Ruby"=>"@v ||= {  }",
 "RubyParser"=>s(:op_asgn_or, s(:ivar, :@v), s(:iasgn, :@v, s(:hash)))}
end

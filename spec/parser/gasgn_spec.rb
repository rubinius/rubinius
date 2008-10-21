def test_case
{"RawParseTree"=>[:gasgn, :$x, [:lit, 42]],
 "Ruby"=>"$x = 42",
 "RubyParser"=>s(:gasgn, :$x, s(:lit, 42))}
end

def test_case
{"RawParseTree"=>[:array, [:str, "a"], [:str, "\#{@b}"], [:str, "c"]],
 "Ruby"=>"%w[a \#{@b} c]",
 "RubyParser"=>s(:array, s(:str, "a"), s(:str, "\#{@b}"), s(:str, "c")),
 "Ruby2Ruby"=>"[\"a\", \"\\\#{@b}\", \"c\"]"}
end

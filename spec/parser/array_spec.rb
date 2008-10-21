def test_case
{"RawParseTree"=>[:array, [:lit, 1], [:lit, :b], [:str, "c"]],
 "Ruby"=>"[1, :b, \"c\"]",
 "RubyParser"=>s(:array, s(:lit, 1), s(:lit, :b), s(:str, "c"))}
end

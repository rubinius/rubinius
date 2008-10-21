def test_case
{"RawParseTree"=>[:lasgn, :var, [:array, [:str, "foo"], [:str, "bar"]]],
 "Ruby"=>"var = [\"foo\", \"bar\"]",
 "RubyParser"=>s(:lasgn, :var, s(:array, s(:str, "foo"), s(:str, "bar")))}
end

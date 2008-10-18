def test_case
{"RawParseTree"=>[:if, [:true], nil, [:if, [:false], [:return], nil]],
 "Ruby"=>"return if false unless true",
 "ParseTree"=>s(:if, s(:true), nil, s(:if, s(:false), s(:return), nil))}
end

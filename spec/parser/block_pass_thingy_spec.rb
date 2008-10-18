def test_case
{"RawParseTree"=>
  [:block_pass,
   [:vcall, :block],
   [:call, [:vcall, :r], :read_body, [:array, [:vcall, :dest]]]],
 "Ruby"=>"r.read_body(dest, &block)",
 "ParseTree"=>
  s(:call,
   s(:call, nil, :r, s(:arglist)),
   :read_body,
   s(:arglist,
    s(:call, nil, :dest, s(:arglist)),
    s(:block_pass, s(:call, nil, :block, s(:arglist)))))}
end

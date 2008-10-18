def test_case
{"RawParseTree"=>
  [:op_asgn2, [:self], :"Bag=", :"||", [:call, [:const, :Bag], :new]],
 "Ruby"=>"self.Bag ||= Bag.new",
 "ParseTree"=>
  s(:op_asgn2,
   s(:self),
   :"Bag=",
   :"||",
   s(:call, s(:const, :Bag), :new, s(:arglist)))}
end

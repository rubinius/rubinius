describe "Chained assignments with method call without parenthesis" do
  it "assigns the result of the method call" do
    a = b = VariablesSpecs::Chain.without_parenthesis 1
    a.should == 1
    b.should == 1
  end
end

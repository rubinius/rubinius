describe "A Symbol literal" do
  it "can be an empty string" do
    c = :''
    c.class.should == Symbol
    c.inspect.should == ':""'
  end
end
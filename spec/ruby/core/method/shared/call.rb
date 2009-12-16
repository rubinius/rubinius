describe :method_call, :shared => true do
  it "invokes the method with the specified arguments, returning the method's return value" do
    m = 12.method("+")
    m.send(@method, 3).should == 15
    m.send(@method, 20).should == 32

    m = MethodSpecs::Methods.new.method(:attr=)
    m.send(@method, 42).should == 42
  end

  it "raises an ArgumentError when given incorrect number of arguments" do
    lambda {
      MethodSpecs::Methods.new.method(:two_req).send(@method, 1, 2, 3)
    }.should raise_error(ArgumentError)
    lambda {
      MethodSpecs::Methods.new.method(:two_req).send(@method, 1)
    }.should raise_error(ArgumentError)
  end
end

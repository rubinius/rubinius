shared :method_call do |cmd|
  describe "Method##{cmd}" do
    it "invokes the method with the specified arguments, returning the method's return value" do
      m = 12.method("+")
      m.send(cmd, 3).should == 15
      m.send(cmd, 20).should == 32
    end
  
    it "raises an ArgumentError when given incorrect number of arguments" do
      lambda {
        MethodSpecs::Methods.new.method(:two_req).send(cmd, 1, 2, 3)
      }.should raise_error(ArgumentError)
      lambda {
        MethodSpecs::Methods.new.method(:two_req).send(cmd, 1)
      }.should raise_error(ArgumentError)
    end
  end
end

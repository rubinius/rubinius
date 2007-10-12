@method_call = shared "Method#call" do |cmd|
  describe "Method##{cmd}" do
    it "invokes the method with the specified arguments, returning the method's return value" do
      m = 12.method("+")
      m.send(cmd, 3).should == 15
      m.send(cmd, 20).should == 32
    end
  
    it "raises an ArgumentError when given incorrect number of arguments" do
      should_raise(ArgumentError, "wrong number of arguments (3 for 2)") do
        MethodSpecs::Methods.new.method(:two).send(cmd, 1, 2, 3)
      end
  
      should_raise(ArgumentError, "wrong number of arguments (1 for 2)") do
        MethodSpecs::Methods.new.method(:two).send(cmd, 1)
      end
    end
  end
end

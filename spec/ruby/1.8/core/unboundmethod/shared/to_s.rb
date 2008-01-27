shared :unboundmethod_to_s do |cmd|
  describe "UnboundMethod##{cmd}" do
    before :each do
      @from_module = UnboundMethodSpecs::Methods.instance_method(:foo)
      @from_method = UnboundMethodSpecs::Methods.new.method(:foo).unbind
    end

    it "returns a String" do
      @from_module.send(cmd).class.should == String
      @from_method.send(cmd).class.should == String
    end

    it "constructs the string to indicate this is an UnboundMethod object" do
      @from_module.send(cmd).should =~ /\bUnboundMethod\b/
      @from_method.send(cmd).should =~ /\bUnboundMethod\b/
    end

    it "constructs the string to indicate the Module and method name extracted from" do
      @from_module.send(cmd).should =~ /\bUnboundMethodSpecs::Methods#foo\b/
      @from_method.send(cmd).should =~ /\bUnboundMethodSpecs::Methods#foo\b/
    end
  end
end

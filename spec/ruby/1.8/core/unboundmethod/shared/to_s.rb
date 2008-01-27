shared :unboundmethod_to_s do |cmd|
  describe "UnboundMethod##{cmd}" do
    it "returns a string representing the UnboundMethod" do
      s = UnboundMethodSpecs::Methods.instance_method(:foo).send(cmd)
      s.should =~ /#<UnboundMethod(:)? UnboundMethodSpecs::Methods#foo>/
      s = UnboundMethodSpecs::Methods.new.method(:foo).unbind.send(cmd)
      s.should =~ /#<UnboundMethod(:)? UnboundMethodSpecs::Methods#foo>/
    end
  end
end

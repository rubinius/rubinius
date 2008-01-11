shared :method_to_s do |cmd|
  describe "Method##{cmd}" do
    it "returns a string representing the method" do
      compliant_on :ruby, :jruby do
        11.method("+").send(cmd).should == "#<Method: Fixnum#+>"
      end

      deviates_on :rubinius do
        11.method("+").send(cmd).should == "#<Method Fixnum(Fixnum)#+>"
      end
    end
  end
end

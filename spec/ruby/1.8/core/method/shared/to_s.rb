shared :method_to_s do |cmd|
  describe "Method##{cmd}" do
    it "returns a string representing the method" do
      11.method("+").send(cmd).should =~ /#<Method(:)? Fixnum(\(Fixnum\))?#\+>/
    end
  end
end

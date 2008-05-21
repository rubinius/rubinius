shared :proc_dup do |cmd|
  describe "Proc##{cmd}" do
    it "returns a copy of self" do
      a = lambda { "hello" }
      b = a.send(cmd)
      
      a.should_not equal(b)
      
      a.call.should == b.call
    end
  end
end

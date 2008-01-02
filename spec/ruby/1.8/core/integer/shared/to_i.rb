shared :integer_to_i do |cmd|
  describe "Integer##{cmd}" do
    it "returns self" do
      a = 1
      a.send(cmd).should == 1 
      a.send(cmd).eql?(a).should == true
    end
  end
end

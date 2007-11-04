shared :regexp_eql do |cmd|
  describe "Regexp##{cmd}" do
    it "is true if self and other have the same pattern" do
      /abc/.send(cmd, /abc/).should == true
      /abc/.send(cmd, /abd/).should == false
    end

    it "is true if self and other have the same character set code" do
      /abc/.send(cmd, /abc/x).should == false
      /abc/x.send(cmd, /abc/x).should == true
      /abc/u.send(cmd, /abc/n).should == false
      /abc/u.send(cmd, /abc/u).should == true
      /abc/n.send(cmd, /abc/n).should == true
    end

    it "is true if other has the same #casefold? values" do
      /abc/.send(cmd, /abc/i).should == false
      /abc/i.send(cmd, /abc/i).should == true
    end
  end
end

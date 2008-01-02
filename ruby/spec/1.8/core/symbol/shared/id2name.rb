shared :symbol_id2name do |cmd|
  describe "Symbol\##{cmd}" do
    it "returns the string corresponding to self" do
      :rubinius.send(cmd).should == "rubinius"
      :squash.send(cmd).should == "squash"
      :[].send(cmd).should == "[]"
      :@ruby.send(cmd).should == "@ruby"
      :@@ruby.send(cmd).should == "@@ruby"
    end
  end
end

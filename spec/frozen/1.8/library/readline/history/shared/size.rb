shared :readline_history_size do |cmd|
  describe "Readline::HISTORY.#{cmd}" do
    it "returns the size of the history" do
      Readline::HISTORY.send(cmd).should == 0
      Readline::HISTORY.push("1", "2", "")
      Readline::HISTORY.send(cmd).should == 3
      
      Readline::HISTORY.pop
      Readline::HISTORY.send(cmd).should == 2
      
      Readline::HISTORY.pop
      Readline::HISTORY.pop
      Readline::HISTORY.send(cmd).should == 0
    end
  end
end
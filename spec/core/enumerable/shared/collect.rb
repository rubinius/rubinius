shared :enumerable_collect do |cmd|
  describe "Enumerable##{cmd}" do
    
    it "collect should return a new array with the results of passing each element to block" do
      entries = [0, 1, 3, 4, 5, 6]
      numerous = EnumerableSpecs::Numerous.new(*entries)
      numerous.send(cmd) { |i| i % 2 }.should == [0, 1, 1, 0, 1, 0]
      numerous.send(cmd) { |i| i }.should == entries
    end 
  end
end

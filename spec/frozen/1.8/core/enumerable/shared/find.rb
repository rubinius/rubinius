shared :enumerable_find do |cmd|
  describe "Enumerable##{cmd}" do 
    # #detect and #find are aliases, so we only need one function 
    before :each do
      @elements = [2, 4, 6, 8, 10]
      @numerous = EnumerableSpecs::Numerous.new(*@elements)
    end
    
    it "Passes each entry in enum to block while block when block is false" do
      visited_elements = []
      @numerous.send(cmd) do |element|
        visited_elements << element
        false
      end
      visited_elements.should == @elements
    end
    
    it "Returns nil when the block is false and there is no ifnone proc given" do
      @numerous.send(cmd) {|e| false }.should == nil
    end
    
    it "Returns the first element for which the block is not false" do
      @elements.each do |element|
        @numerous.send(cmd) {|e| e > element - 1 }.should == element
      end
    end
    
    it "Returns the value of the ifnone proc if the block is false" do
      fail_proc = lambda { "cheeseburgers" }
      @numerous.send(cmd, fail_proc) {|e| false }.should == "cheeseburgers"
    end
    
    it "Doesn't call the ifnone proc if an element is found" do
      fail_proc = lambda { raise "This shouldn't have been called" }
      @numerous.send(cmd, fail_proc) {|e| e == @elements.first }
    end
    
    it "Calls the ifnone proc only once when the block is false" do
      times = 0
      fail_proc = lambda { times += 1; raise if times > 1; "cheeseburgers" }
      @numerous.send(cmd, fail_proc) {|e| false }.should == "cheeseburgers"
    end
  end
end

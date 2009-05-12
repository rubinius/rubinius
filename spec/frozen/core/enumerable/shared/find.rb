describe :enumerable_find, :shared => true do
  # #detect and #find are aliases, so we only need one function 
  before :each do
    @elements = [2, 4, 6, 8, 10]
    @numerous = EnumerableSpecs::Numerous.new(*@elements)
  end
  
  it "passes each entry in enum to block while block when block is false" do
    visited_elements = []
    @numerous.send(@method) do |element|
      visited_elements << element
      false
    end
    visited_elements.should == @elements
  end
  
  it "returns nil when the block is false and there is no ifnone proc given" do
    @numerous.send(@method) {|e| false }.should == nil
  end
  
  it "returns the first element for which the block is not false" do
    @elements.each do |element|
      @numerous.send(@method) {|e| e > element - 1 }.should == element
    end
  end
  
  it "returns the value of the ifnone proc if the block is false" do
    fail_proc = lambda { "cheeseburgers" }
    @numerous.send(@method, fail_proc) {|e| false }.should == "cheeseburgers"
  end
  
  it "doesn't call the ifnone proc if an element is found" do
    fail_proc = lambda { raise "This shouldn't have been called" }
    @numerous.send(@method, fail_proc) {|e| e == @elements.first }.should == 2
  end
  
  it "calls the ifnone proc only once when the block is false" do
    times = 0
    fail_proc = lambda { times += 1; raise if times > 1; "cheeseburgers" }
    @numerous.send(@method, fail_proc) {|e| false }.should == "cheeseburgers"
  end
  
  ruby_version_is "" ... "1.8.7" do
    it "raises a LocalJumpError if no block given" do
      lambda { @numerous.send(@method) }.should raise_error(LocalJumpError)
    end
  end
  ruby_version_is "1.8.7" do
    it "returns an enumerator when no block given" do
      @numerous.send(@method).should be_kind_of(enumerator_class)
    end
    
    it "passes the ifnone proc to the enumerator" do
      times = 0
      fail_proc = lambda { times += 1; raise if times > 1; "cheeseburgers" }
      @numerous.send(@method, fail_proc).each {|e| false }.should == "cheeseburgers"
    end
  end
  
end

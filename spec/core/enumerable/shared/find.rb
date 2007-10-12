@enumerable_find = shared "Enumerable#find" do |cmd|
  describe "Enumerable##{cmd}" do 
    # #detect and #find are aliases, so we only need one function 
    before :each do
      @a = EachDefiner.new( 2, 4, 6, 8, 10 )
      @fail_count = 0
      @fail_proc = lambda { @fail_count += 1 ; "not found" }
      @fail_proc_value = "not found"
       
    end
    
    after :each do 
      @fail_count = 0
    end
    
    it "Returns the first for which block is not false or call ifnone proc, empty array and nil" do
      args, fail_value = [], nil      
      EachDefiner.new().send(cmd, *args) {|a| true  }.should == fail_value
      @a.send(cmd, *args) {|a| false }.should == fail_value
      @a.send(cmd, *args) {|a| a > 1 }.should == 2
      @a.send(cmd, *args) {|a| a > 5 }.should == 6
      @a.send(cmd, *args) {|a| a > 9 }.should == 10
      @a.send(cmd, *args) {|a| a > 10 }.should == fail_value 
    end
    
    it "returns the first for which block is not false, or call ifnone proc" do
      args, fail_value = @fail_proc,  @fail_proc_value 
      EachDefiner.new().send(cmd, *args) {|a| true  }.should == fail_value
      @a.send(cmd, *args) {|a| false }.should == fail_value
      @a.send(cmd, *args) {|a| a > 1 }.should == 2
      @a.send(cmd, *args) {|a| a > 5 }.should == 6
      @a.send(cmd, *args) {|a| a > 9 }.should == 10
      @a.send(cmd, *args) {|a| a > 10 }.should == fail_value 
    end
    
    it "call only once if " do
      # Make sure that the "proc" is only called once, and only if no
      # match is found.       
      @fail_count = 0
      @a.send(cmd, @fail_proc) {|a| false }.should == @fail_proc_value
      @a.send(cmd, @fail_proc) {|a| true }.should_not == @fail_proc_value
      @fail_count.should == 1
    end
    
    it "not call the ifnone proc when find a element" do
      @fail_count = 0
      @a.send(cmd, @fail_proc) {|a| true }.should == 2
      @fail_count.should == 0
    end  
  
    it "find should be a synonym for detect" do
      Numerous.new.send(cmd) { |i| i > 3 }.should == Numerous.new.detect { |i| i > 3 }
      Numerous.new.send(cmd) { |i| i > 3 }.should == Numerous.new.find { |i| i > 3 } 
    end
  end
end

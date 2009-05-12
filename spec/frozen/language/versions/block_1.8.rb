describe "Block parameters" do
  it "assign to local variable" do
    i = 0
    a = [1,2,3]
    a.each {|i| ;}
    i.should == 3
  end

  it "captures variables from the outer scope" do
    a = [1,2,3]
    sum = 0
    var = nil
    a.each {|var| sum += var}
    sum.should == 6
    var.should == 3
  end
end

describe "Block parameters (to be removed from MRI)" do
  it "assigns to a global variable" do
    $global_for_block_assignment = 0
    a = [1,2,3]
    a.each {|$global_for_block_assignment| ;}
    $global_for_block_assignment.should == 3
  end

  it "calls method=" do
    class T
      def n; return @n; end
      def n=(val); @n = val + 1; end
      def initialize; @n = 0; end
    end
    t = T.new
    t.n.should == 0
    a = [1,2,3]
    a.each {|t.n| }
    t.n.should == 4
  end
end
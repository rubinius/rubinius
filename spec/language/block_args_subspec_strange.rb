
# This functionality is being removed from MRI and has never
# been used in 1.8, therefore rubinius doesn't support
# it already.
describe "Block args" do
  it "should assign to a global variable" do
    $global_for_block_assignment = 0
    a = [1,2,3]
    a.each {|$global_for_block_assignment| ;}
    $global_for_block_assignment.should == 3
  end

  it "should call method=" do
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

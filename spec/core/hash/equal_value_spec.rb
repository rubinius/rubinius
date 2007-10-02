require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Hash#==" do
  it "returns true if other Hash has the same number of keys and each key-value pair matches" do
    Hash.new(5).should == Hash.new(1)
    Hash.new {|h, k| 1}.should == Hash.new {}
    Hash.new {|h, k| 1}.should == Hash.new(2)
    
    a = {:a => 5}
    b = {}

    a.should_not == b

    b[:a] = 5

    a.should == b

    c = Hash.new {|h, k| 1}
    d = Hash.new {}
    c[1] = 2
    d[1] = 2
    c.should == d
  end
  
  version :not, '1.8.6' do
    it "calls to_hash on its argument" do
      obj = Object.new
      obj.should_receive(:to_hash, :returning => {1 => 2, 3 => 4})
    
      {1 => 2, 3 => 4}.should == obj
    end
  end
  
  it "does not call to_hash on hash subclasses" do
    {5 => 6}.should == ToHashHash[5 => 6]
  end
  
  it "returns false when the numbers of keys differ without comparing any elements" do
    obj = Object.new
    obj.should_not_receive(:==)
    obj.should_not_receive(:eql?)

    {}.should_not == { obj => obj }
    { obj => obj }.should_not == {}
  end
  
  it "compares keys with eql? semantics" do
    { 1.0 => "x" }.should == { 1.0 => "x" }
    { 1 => "x" }.should_not == { 1.0 => "x" }
    { 1.0 => "x" }.should_not == { 1 => "x" }
  end

  it "first compares keys via hash" do
    # Can't use should_receive because it uses hash internally
    x = Object.new
    def x.hash() freeze; 0 end
    y = Object.new
    def y.hash() freeze; 0 end

    { x => 1 } == { y => 1 }
    x.frozen?.should == true
    y.frozen?.should == true
  end
    
  it "does not compare keys with different hash codes via eql?" do
    # Can't use should_receive because it uses hash and eql? internally
    x = Object.new
    def x.eql?(o) raise("Shouldn't receive eql?") end
    y = Object.new
    def y.eql?(o) raise("Shouldn't receive eql?") end

    def x.hash() freeze; 0 end
    def y.hash() freeze; 1 end

    { x => 1 }.should_not == { y => 1 }
    x.frozen?.should == true
    y.frozen?.should == true
  end    
    
  it "compares keys with matching hash codes via eql?" do
    # Can't use should_receive because it uses hash and eql? internally
    a = Array.new(2) do 
      obj = Object.new

      def obj.hash()
        # It's undefined whether the impl does a[0].eql?(a[1]) or
        # a[1].eql?(a[0]) so we taint both.
        def self.eql?(o) taint; o.taint; false; end
        return 0
      end

      obj
    end

    { a[0] => 1 }.should_not == { a[1] => 1 }
    a[0].tainted?.should == true
    a[1].tainted?.should == true

    a = Array.new(2) do 
      obj = Object.new

      def obj.hash()
        # It's undefined whether the impl does a[0].eql?(a[1]) or
        # a[1].eql?(a[0]) so we taint both.
        def self.eql?(o) taint; o.taint; true; end
        return 0
      end

      obj
    end

    { a[0] => 1 }.should == { a[1] => 1 }
    a[0].tainted?.should == true
    a[1].tainted?.should == true
  end
  
  it "compares values with == semantics" do
    { "x" => 1.0 }.should == { "x" => 1 }
  end
  
  it "does not compare values when keys don't match" do
    value = Object.new
    value.should_not_receive(:==)
    { 1 => value }.should_not == { 2 => value }
  end
  
  it "compares values when keys match" do
    x = Object.new
    y = Object.new
    def x.==(o) freeze; false; end
    def y.==(o) freeze; false; end
    { 1 => x }.should_not == { 1 => y }
    # There is no order
    (x.frozen? | y.frozen?).should == true

    x = Object.new
    y = Object.new
    def x.==(o) freeze; true; end
    def y.==(o) freeze; true; end
    { 1 => x }.should == { 1 => y }
    # There is no order
    (x.frozen? | y.frozen?).should == true
  end

  it "ignores hash class differences" do
    h = { 1 => 2, 3 => 4 }
    MyHash[h].should == h
    MyHash[h].should == MyHash[h]
    h.should == MyHash[h]
  end  
end

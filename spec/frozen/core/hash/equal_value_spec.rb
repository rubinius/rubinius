require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Hash#==" do
  it "returns true if other Hash has the same number of keys and each key-value pair matches" do
    new_hash(5).should == new_hash(1)
    new_hash {|h, k| 1}.should == new_hash {}
    new_hash {|h, k| 1}.should == new_hash(2)

    a = new_hash(:a => 5)
    b = new_hash
    a.should_not == b

    b[:a] = 5
    a.should == b

    c = new_hash("a" => 5)
    a.should_not == c

    d = new_hash {|h, k| 1}
    e = new_hash {}
    d[1] = 2
    e[1] = 2
    d.should == e
  end

  it "does not call to_hash on hash subclasses" do
    new_hash(5 => 6).should == ToHashHash[5 => 6]
  end

  it "returns false when the numbers of keys differ without comparing any elements" do
    obj = mock('x')
    h = new_hash(obj => obj)

    obj.should_not_receive(:==)
    obj.should_not_receive(:eql?)

    new_hash.should_not == h
    h.should_not == new_hash
  end

  it "compares keys with eql? semantics" do
    new_hash(1.0 => "x").should == new_hash(1.0 => "x")
    new_hash(1 => "x").should_not == new_hash(1.0 => "x")
    new_hash(1.0 => "x").should_not == new_hash(1 => "x")
  end

  it "first compares keys via hash" do
    # Can't use should_receive because it uses hash internally
    x = mock('x')
    def x.hash() 0 end
    y = mock('y')
    def y.hash() 0 end

    new_hash(x => 1).should_not == new_hash(y => 1)
  end

  it "does not compare keys with different hash codes via eql?" do
    # Can't use should_receive because it uses hash and eql? internally
    x = mock('x')
    y = mock('y')
    x.instance_variable_set(:@other, y)
    y.instance_variable_set(:@other, x)
    def x.eql?(o)
      raise("x Shouldn't receive eql?") if o == @other
      self == o
    end
    def y.eql?(o)
      raise("y Shouldn't receive eql?") if o == @other
      self == o
    end

    def x.hash() 0 end
    def y.hash() 1 end

    new_hash(x => 1).should_not == new_hash(y => 1)
  end

  it "compares keys with matching hash codes via eql?" do
    # Can't use should_receive because it uses hash and eql? internally
    a = Array.new(2) do
      obj = mock('0')

      def obj.hash()
        return 0
      end
      # It's undefined whether the impl does a[0].eql?(a[1]) or
      # a[1].eql?(a[0]) so we taint both.
      def obj.eql?(o)
        return true if self == o
        taint
        o.taint
        false
      end

      obj
    end

    new_hash(a[0] => 1).should_not == new_hash(a[1] => 1)
    a[0].tainted?.should == true
    a[1].tainted?.should == true

    a = Array.new(2) do
      obj = mock('0')

      def obj.hash()
        # It's undefined whether the impl does a[0].eql?(a[1]) or
        # a[1].eql?(a[0]) so we taint both.
        def self.eql?(o) taint; o.taint; true; end
        return 0
      end

      obj
    end

    new_hash(a[0] => 1).should == new_hash(a[1] => 1)
    a[0].tainted?.should == true
    a[1].tainted?.should == true
  end

  it "computes equality for recursive hashes" do
    h = new_hash
    h[:a] = h
    h.eql?(h[:a]).should == true
    (h == h[:a]).should == true
  end

  it "compares values with == semantics" do
    new_hash("x" => 1.0).should == new_hash("x" => 1)
  end

  it "does not compare values when keys don't match" do
    value = mock('x')
    value.should_not_receive(:==)
    new_hash(1 => value).should_not == new_hash(2 => value)
  end

  it "compares values when keys match" do
    x = mock('x')
    y = mock('y')
    def x.==(o) false end
    def y.==(o) false end
    new_hash(1 => x).should_not == new_hash(1 => y)

    x = mock('x')
    y = mock('y')
    def x.==(o) true end
    def y.==(o) true end
    new_hash(1 => x).should == new_hash(1 => y)
  end

  it "ignores hash class differences" do
    h = new_hash(1 => 2, 3 => 4)
    MyHash[h].should == h
    MyHash[h].should == MyHash[h]
    h.should == MyHash[h]
  end
end

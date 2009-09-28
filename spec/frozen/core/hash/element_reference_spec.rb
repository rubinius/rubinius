require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Hash#[]" do
  it "returns the value for key" do
    obj = mock('x')
    h = new_hash(1 => 2, 3 => 4, "foo" => "bar", obj => obj, [] => "baz")
    h[1].should == 2
    h[3].should == 4
    h["foo"].should == "bar"
    h[obj].should == obj
    h[[]].should == "baz"
  end

  it "returns nil as default default value" do
    new_hash(0 => 0)[5].should == nil
  end

  it "returns the default (immediate) value for missing keys" do
    h = new_hash 5
    h[:a].should == 5
    h[:a] = 0
    h[:a].should == 0
    h[:b].should == 5
  end

  it "calls subclass implementations of default" do
    h = DefaultHash.new
    h[:nothing].should == 100
  end

  it "does not create copies of the immediate default value" do
    str = "foo"
    h = new_hash(str)
    a = h[:a]
    b = h[:b]
    a << "bar"

    a.should equal(b)
    a.should == "foobar"
    b.should == "foobar"
  end

  it "returns the default (dynamic) value for missing keys" do
    h = new_hash { |hsh, k| k.kind_of?(Numeric) ? hsh[k] = k + 2 : hsh[k] = k }
    h[1].should == 3
    h['this'].should == 'this'
    h.should == new_hash(1 => 3, 'this' => 'this')

    i = 0
    h = new_hash { |hsh, key| i += 1 }
    h[:foo].should == 1
    h[:foo].should == 2
    h[:bar].should == 3
  end

  it "does not return default values for keys with nil values" do
    h = new_hash 5
    h[:a] = nil
    h[:a].should == nil

    h = new_hash() { 5 }
    h[:a] = nil
    h[:a].should == nil
  end

  it "compares keys with eql? semantics" do
    new_hash(1.0 => "x")[1].should == nil
    new_hash(1.0 => "x")[1.0].should == "x"
    new_hash(1 => "x")[1.0].should == nil
    new_hash(1 => "x")[1].should == "x"
  end

  it "compares key via hash" do
    x = mock('0')
    x.should_receive(:hash).and_return(0)

    h = new_hash
    # 1.9 only calls #hash if the hash had at least one entry beforehand.
    h[:foo] = :bar
    h[x].should == nil
  end

  it "does not compare key with unknown hash codes via eql?" do
    x = mock('x')
    y = mock('y')
    def x.eql?(o) raise("Shouldn't receive eql?") end

    x.should_receive(:hash).and_return(0)
    y.should_receive(:hash).and_return(1)

    new_hash(y => 1)[x].should == nil
  end

  it "compares key with found hash code via eql?" do
    y = mock('0')
    y.should_receive(:hash).twice.and_return(0)

    x = mock('0')
    def x.hash()
      def self.eql?(o) taint; false; end
      return 0
    end

    new_hash(y => 1)[x].should == nil
    x.tainted?.should == true

    x = mock('0')
    def x.hash()
      def self.eql?(o) taint; true; end
      return 0
    end

    new_hash(y => 1)[x].should == 1
    x.tainted?.should == true
  end
end

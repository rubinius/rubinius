require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel#freeze" do
  it "prevents self from being further modified" do
    o = mock('o')
    o.frozen?.should == false
    o.freeze
    o.frozen?.should == true
  end

  # 1.9 allows immediate's to be frozen; reported as bug #1747
  it "has no effect on immediate values" do
    a = nil
    b = true
    c = false
    d = 1
    a.freeze
    b.freeze
    c.freeze
    d.freeze
    a.frozen?.should == false
    b.frozen?.should == false
    c.frozen?.should == false
    d.frozen?.should == false
  end

  ruby_version_is "" ... "1.9" do
    it "causes mutative calls to raise TypeError" do
      o = Class.new do
        def mutate; @foo = 1; end
      end.new
      o.freeze
      lambda {o.mutate}.should raise_error(TypeError)
    end
  end

  ruby_version_is "1.9" do
    it "causes mutative calls to raise RuntimeError" do
      o = Class.new do
        def mutate; @foo = 1; end
      end.new
      o.freeze
      lambda {o.mutate}.should raise_error(RuntimeError)
    end
  end
end

require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Hash#default_proc" do
  it "returns the block passed to Hash.new" do
    h = new_hash { |i| 'Paris' }
    p = h.default_proc
    p.call(1).should == 'Paris'
  end

  it "returns nil if no block was passed to proc" do
    new_hash.default_proc.should == nil
  end
end

describe "Hash#default_proc=" do
  ruby_version_is "1.9" do
    it "replaces the block passed to Hash.new" do
      h = new_hash { |i| 'Paris' }
      h.default_proc = Proc.new { 'Montreal' }
      p = h.default_proc
      p.call(1).should == 'Montreal'
    end

    it "uses :to_proc on its argument" do
      h = new_hash { |i| 'Paris' }
      obj = mock('to_proc')
      obj.should_receive(:to_proc).and_return(Proc.new { 'Montreal' })
      (h.default_proc = obj).should equal(obj)
      h[:cool_city].should == 'Montreal'
    end

    it "overrides the static default" do
      h = new_hash(42)
      h.default_proc = Proc.new { 6 }
      h.default.should be_nil
      h.default_proc.call.should == 6
    end

    it "raise an error if passed stuff not convertible to procs" do
      lambda{new_hash.default_proc = nil}.should raise_error(TypeError)
      lambda{new_hash.default_proc = 42}.should raise_error(TypeError)
    end
  end
end

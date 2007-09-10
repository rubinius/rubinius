require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Hash#index" do
  it "returns the corresponding key for value" do
    {2 => 'a', 1 => 'b'}.index('b').should == 1
  end
  
  it "returns nil if the value is not found" do
    {:a => -1, :b => 3.14, :c => 2.718}.index(1).should == nil
    Hash.new(5).index(5).should == nil
  end

  it "compares values using ==" do
    {1 => 0}.index(0.0).should == 1
    {1 => 0.0}.index(0).should == 1
    
    needle = Object.new
    inhash = Object.new
    inhash.should_receive(:==, :with => [needle], :returning => true)
    
    {1 => inhash}.index(needle).should == 1
  end

  it "compares values with same order as keys() and values()" do
    h = {1 => 0, 2 => 0, 3 => 0, 4 => 0, 5 => 0, 6 => 0}
    h.index(0).should == h.keys.first
    
    needle = Object.new
    h = {1 => Object.new, 3 => Object.new, 4 => Object.new, 42 => Object.new }
    h.values[0].should_receive(:==, :with => [needle], :returning => false)
    h.values[1].should_receive(:==, :with => [needle], :returning => false)
    h.values[2].should_receive(:==, :with => [needle], :returning => true)
    h.values[3].should_not_receive(:==)
    
    h.index(needle).should == h.keys[2]
  end
end

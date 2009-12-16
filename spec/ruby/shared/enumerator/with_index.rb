require File.dirname(__FILE__) + '/../../spec_helper'

describe :enum_with_index, :shared => true do

  require File.dirname(__FILE__) + '/../../fixtures/enumerator/classes'
  
  before :each do
    @enum = [1, 2, 3, 4].to_enum
  end 
  
  it "passes each element and its index to block" do
    @a = []
    @enum.send(@method) { |o, i| @a << [o, i] }
    @a.should == [[1, 0], [2, 1], [3, 2], [4, 3]]
  end
  
  it "returns the object being enumerated when given a block" do
    [1, 2, 3, 4].should == @enum.send(@method) { |o, i| :glark }
  end

  it "binds splat arguments properly" do
    acc = []
    @enum.send(@method) { |*b| c,d = b; acc << c; acc << d }
    [1, 0, 2, 1, 3, 2, 4, 3].should == acc
  end
  
  ruby_version_is '1.8.7' do
    it "returns an enumerator if no block is supplied" do 
      ewi = @enum.send(@method)
      ewi.should be_kind_of(enumerator_class)
      ewi.to_a.should == [[1, 0], [2, 1], [3, 2], [4, 3]]
    end    
  end
  
end

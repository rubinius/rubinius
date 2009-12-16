require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/take'

describe "Enumerable#first" do
  ruby_version_is '1.8.7' do
    it "returns the first element" do
      EnumerableSpecs::Numerous.new.first.should == 2
      EnumerableSpecs::Empty.new.first.should == nil
    end
  
    it "returns nil if self is empty" do
      EnumerableSpecs::Empty.new.first.should == nil
    end
    
    describe "when passed an argument" do
      it_behaves_like :enumerable_take, :first
    end
  end
end
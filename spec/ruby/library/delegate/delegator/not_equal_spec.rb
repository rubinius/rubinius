require File.expand_path('../../../../spec_helper', __FILE__)
require File.expand_path('../../fixtures/classes', __FILE__)

ruby_version_is "1.9" do
  describe "Delegator#!=" do
    before :each do
      @base = mock('base')
      @delegator = DelegateSpecs::Delegator.new(@base)
    end

    it "is not delegated when passed self" do
      @base.should_not_receive(:"!=")
      (@delegator != @delegator).should be_false
    end

    it "is delegated when passed the delegated object" do
      @base.should_receive(:"!=").and_return(true)
      (@delegator != @base).should be_true
    end

    it "is delegated in general" do
      @base.should_receive(:"!=").and_return(false)
      (@delegator != 42).should be_false
    end
  end
end

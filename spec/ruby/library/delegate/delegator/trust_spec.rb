require File.expand_path('../../../../spec_helper', __FILE__)
require File.expand_path('../../fixtures/classes', __FILE__)

ruby_version_is "1.9" do
  describe "Delegator#trust" do
    before :each do
      @delegate = lambda{$SAFE=4; DelegateSpecs::Delegator.new([])}.call
    end

    it "returns self" do
      @delegate.trust.equal?(@delegate).should be_true
    end

    it "trusts the delegator" do
      @delegate.trust
      @delegate.untrusted?.should be_false
      lambda{$SAFE=4; @delegate.data = :foo }.should raise_error( SecurityError )
    end

    it "trusts the delegated object" do
      @delegate.trust
      @delegate.__getobj__.untrusted?.should be_false
      lambda{$SAFE=4; @delegate << 42}.should raise_error( SecurityError )
    end
  end
end
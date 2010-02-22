require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Kernel#trust" do
  ruby_version_is "1.9" do
    it "clears the untrusted bit from a target object" do
      o = mock('o')
      o.untrusted?.should == false
      o.untrust
      o.untrusted?.should == true
      o.trust
      o.untrusted?.should == false
    end

    it "can not be called on frozen object" do
      o = mock('o')
      o.untrust
      lambda {o.trust}.should_not raise_error
      o.untrust
      o.freeze
      lambda {o.trust}.should raise_error(RuntimeError)
    end
  end
end

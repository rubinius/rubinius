require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Kernel#untrusted?" do
  ruby_version_is "1.9" do
    it "sets the untrusted bit on a target object" do
      o = mock('o')
      o.untrusted?.should == false
      o.untrust
      o.untrusted?.should == true
    end

    it "can not be called on frozen object" do
      o = mock('o')
      lambda {o.untrust}.should_not raise_error
      o.trust
      o.freeze
      lambda {o.untrust}.should raise_error(RuntimeError)
    end
  end
end

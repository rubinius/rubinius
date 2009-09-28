require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

ruby_version_is "1.8.7" do
  describe "Kernel#tap" do
    it "always yields self and returns self" do
      a = KernelSpecs::A.new
      a.tap{|o| o.should equal(a); 42}.should equal(a)
    end
    
    it "raises a LocalJumpError when no block given" do
      lambda { 3.tap }.should raise_error(LocalJumpError)
    end  
  end
end

require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

ruby_version_is ""..."1.9" do
  describe "Kernel#getc" do
    it "is a private method" do
      Kernel.should have_private_instance_method(:getc)
    end
  end

  describe "Kernel.getc" do
    it "needs to be reviewed for spec completeness"
  end
end

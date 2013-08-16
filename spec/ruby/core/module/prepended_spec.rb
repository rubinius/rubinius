# -*- encoding: us-ascii -*-

require File.expand_path('../../../spec_helper', __FILE__)

ruby_version_is "2.0" do
  describe "Module#prepended" do
    it "is a private method" do
      Module.should have_private_instance_method(:prepended, true)
    end
  end
end

require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/each_byte'

ruby_version_is "1.8.7" do
  describe "ARGF.bytes" do
    it_behaves_like :argf_each_byte, :bytes
  end
end
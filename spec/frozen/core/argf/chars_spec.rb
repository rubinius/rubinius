require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/each_char'

ruby_version_is "1.8.7" do
  describe "ARGF.chars" do
    it_behaves_like :argf_each_char, :chars
  end
end
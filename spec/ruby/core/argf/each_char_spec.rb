require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/each_char'

ruby_version_is "1.8.7" do
  describe "ARGF.each_char" do
    it_behaves_like :argf_each_char, :each_char
  end
end
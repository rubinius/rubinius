require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../shared/each_codepoint', __FILE__)

ruby_version_is "2.0" do
  describe "ARGF.each_codepoint" do
    it_behaves_like :argf_each_codepoint, :each_codepoint
  end
end

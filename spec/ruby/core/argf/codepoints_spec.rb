require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../shared/each_codepoint', __FILE__)

ruby_version_is "2.0" do
  describe "ARGF.codepoints" do
    it_behaves_like :argf_each_codepoint, :codepoints
  end
end

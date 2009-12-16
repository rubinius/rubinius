require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/codepoints'

ruby_version_is "1.9" do
  describe "String#each_codepoint" do
    it_behaves_like(:string_codepoints, :each_codepoint)
  end
end

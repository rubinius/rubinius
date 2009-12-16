require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/codepoints'

ruby_version_is "1.9" do
  describe "String#codepoints" do
    it_behaves_like(:string_codepoints, :codepoints)
  end
end

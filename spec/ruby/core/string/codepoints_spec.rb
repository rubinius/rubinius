require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/codepoints'

with_feature :encoding do
  describe "String#codepoints" do
    it_behaves_like(:string_codepoints, :codepoints)
  end
end

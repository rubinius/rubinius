require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/rect'

ruby_version_is "1.9" do
  describe "Numeric#rectangular" do
    it_behaves_like(:numeric_rect, :rectangular)
  end
end

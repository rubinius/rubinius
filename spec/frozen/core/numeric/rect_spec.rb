require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/rect'

ruby_version_is "1.9" do
  describe "Numeric#rect" do
    it_behaves_like(:numeric_rect, :rect)
  end
end

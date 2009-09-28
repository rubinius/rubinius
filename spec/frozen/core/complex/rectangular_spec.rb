require File.dirname(__FILE__) + '/../../spec_helper'

ruby_version_is "1.9" do
  
  require File.dirname(__FILE__) + '/../../shared/complex/rect'
  
  describe "Complex#rectangular" do
    it_behaves_like(:complex_rect, :rectangular)
  end
end

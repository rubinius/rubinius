require File.dirname(__FILE__) + '/../../spec_helper'

# TODO: enable for 1.8.7 as well
ruby_version_is "1.9" do
  require File.dirname(__FILE__) + '/../../shared/enumerator/next'
  
  describe "Enumerator#next" do
    it_behaves_like(:enum_next,:next)
  end
end

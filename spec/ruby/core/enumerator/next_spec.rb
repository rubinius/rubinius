require File.expand_path('../../../spec_helper', __FILE__)

# TODO: enable for 1.8.7 as well
ruby_version_is "1.9" do
  require File.expand_path('../../../shared/enumerator/next', __FILE__)
  
  describe "Enumerator#next" do
    it_behaves_like(:enum_next,:next)
  end
end

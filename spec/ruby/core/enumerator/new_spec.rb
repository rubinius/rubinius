require File.expand_path('../../../spec_helper', __FILE__)

ruby_version_is "1.8.7" do
  require File.expand_path('../../../shared/enumerator/new', __FILE__)

  describe "Enumerator.new" do
    it_behaves_like(:enum_new, :new)
  end
end

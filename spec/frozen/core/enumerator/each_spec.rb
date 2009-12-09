require File.dirname(__FILE__) + '/../../shared/enumerator/each'

ruby_version_is "1.8.7" do
  describe "Enumerator#each" do
    it_behaves_like(:enum_each, :each)
  end
end

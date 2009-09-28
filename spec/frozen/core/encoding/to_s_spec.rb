require File.dirname(__FILE__) + '/shared/name'

ruby_version_is "1.9" do
  describe "Encoding#to_s" do
    it_behaves_like(:encoding_name, :to_s)
  end
end
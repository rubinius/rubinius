require File.dirname(__FILE__) + '/../../shared/complex/inspect'

ruby_version_is "1.9" do
  describe "Complex#inspect" do
    it_behaves_like(:complex_inspect, :inspect)
  end
end

require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/equal'

ruby_version_is "1.9" do
  describe "Proc#eql?" do
    it_behaves_like(:proc_equal, :eql?)
  end
end

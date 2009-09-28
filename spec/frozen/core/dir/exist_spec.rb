require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/exists'

ruby_version_is "1.9" do
  describe "Dir.exist?" do
    it_behaves_like(:dir_exists, :exist?)
  end
end

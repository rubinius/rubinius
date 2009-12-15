require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/exists'

ruby_version_is "1.9" do
  describe "Dir.exist?" do
    before :all do
      DirSpecs.create_mock_dirs
    end

    after :all do
      DirSpecs.delete_mock_dirs
    end

    it_behaves_like(:dir_exists, :exist?)
  end
end

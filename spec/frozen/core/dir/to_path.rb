require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/common'
require File.dirname(__FILE__) + '/shared/path'

ruby_version_is "1.9" do
  describe "Dir#to_path" do
    it_behaves_like(:dir_path, :to_path)
  end
end

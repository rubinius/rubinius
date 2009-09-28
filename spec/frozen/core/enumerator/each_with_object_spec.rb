require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../shared/enumerator/with_object'

ruby_version_is "1.9" do
  describe "Enumerator#each_with_object" do
    it_behaves_like :enum_with_object, :each_with_object
  end
end

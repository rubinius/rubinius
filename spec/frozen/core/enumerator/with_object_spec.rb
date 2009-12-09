require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../shared/enumerator/with_object'

ruby_version_is "1.8.8" do
  describe "Enumerator#with_object" do
    it_behaves_like :enum_with_object, :with_object
  end
end

require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../shared/kernel/singleton_method_added'

ruby_version_is "1.9" do
  describe "BasicObject#singleton_method_added" do
    it_behaves_like(:singleton_method_added, :singleton_method_added, BasicObject)
  end
end

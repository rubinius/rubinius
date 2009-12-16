require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/../../shared/kernel/singleton_method_added'

ruby_version_is ""..."1.9" do
  describe "Kernel#singleton_method_added" do
    it_behaves_like(:singleton_method_added, :singleton_method_added, Kernel)
  end
end

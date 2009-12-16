require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../shared/kernel/singleton_method_removed'

ruby_version_is ""..."1.9" do
  describe "Kernel#singleton_method_removed" do
    it_behaves_like(:singleton_method_removed, :singleton_method_removed, Kernel)
  end
end

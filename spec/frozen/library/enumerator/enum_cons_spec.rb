require File.dirname(__FILE__) + '/../../spec_helper'

ruby_version_is ""..."1.9" do
  require 'enumerator'
  require File.dirname(__FILE__) + '/../../shared/enumerator/enum_cons'
  
  describe "Enumerator#enum_cons" do
    it_behaves_like(:enum_cons, :enum_cons)
  end
end

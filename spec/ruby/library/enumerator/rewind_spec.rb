require File.dirname(__FILE__) + '/../../spec_helper'

ruby_version_is "1.8.7"..."1.9" do  
  require File.dirname(__FILE__) + '/../../shared/enumerator/rewind'
  require 'enumerator'

  describe "Enumerator#rewind" do
    it_behaves_like(:enum_rewind, :rewind)
  end    
end

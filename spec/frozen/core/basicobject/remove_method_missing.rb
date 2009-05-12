require File.dirname(__FILE__) + '/../../spec_helper'

ruby_version_is "1.9".."1.9.9" do

  quarantine! do
    it "shall not segfault by any means" do
      class BasicObject
	remove_method :method_missing
      end
      "Hello World".send( :kabooum_42 ).should raise_error( NoMethodError )
    end
  end

end

require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/each_line'

ruby_version_is "1.8.7" do
  describe "String#lines" do
    it_behaves_like(:string_each_line, :lines)
  end
end

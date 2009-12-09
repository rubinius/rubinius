require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/each_line'

describe "String#each" do
  ruby_version_is ''...'1.9' do
    it_behaves_like(:string_each_line, :each)
  end

  ruby_version_is '1.9' do
    it "raises a NoMethodError" do
      lambda { "gone in 1.9".each }.should raise_error(NoMethodError)
    end
  end

end

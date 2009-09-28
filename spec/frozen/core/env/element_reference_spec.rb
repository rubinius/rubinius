require File.dirname(__FILE__) + '/../../spec_helper'

describe "ENV.[]" do
  before :each do
    @variable_name = 'USER'
    platform_is :windows do
      @variable_name = 'USERNAME'
    end
  end

  it "returns the specified environment variable" do
    ENV[@variable_name].should == username
  end

  it "returns nil if the variable isn't found" do
    ENV["this_var_is_never_set"].should == nil
  end

  ruby_version_is "1.9" do
    it "uses the locale encoding" do
      ENV[@variable_name].encoding.should == Encoding.find('locale')
    end
  end
end

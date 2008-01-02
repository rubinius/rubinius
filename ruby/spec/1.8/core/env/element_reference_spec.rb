require File.dirname(__FILE__) + '/../../spec_helper'

describe "ENV.[]" do
  it "returns the specified environment variable" do
    ENV["USER"].should == `whoami`.strip
  end

  it "returns nil if the variable isn't found" do
    ENV["this_var_is_never_set"].should == nil
  end
end

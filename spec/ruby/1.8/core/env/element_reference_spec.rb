require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "ENV#[]" do
  it "returns the specified environment variable" do
    ENV["USER"].should == EnvSpecs.get_current_user
  end

  it "returns nil if the variable isn't found" do
    ENV["this_var_is_never_set"].should == nil
  end
end

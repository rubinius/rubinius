require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "ENV.clear" do

  it "deletes all environment variables" do
    orig = ENV.to_hash
    begin
      ENV.clear
      EnvSpecs.get_env.should == {}
    ensure
      ENV.replace orig
    end
  end

end

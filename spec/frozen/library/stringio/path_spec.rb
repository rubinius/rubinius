require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "StringIO#path" do
  ruby_version_is "" ... "1.9.2" do
    it "returns nil" do
      StringIO.new("path").path.should be_nil
    end
  end

  ruby_version_is "1.9.2" do
    it "is not defined" do
      lambda { StringIO.new("path").path }.should raise_error(NoMethodError)
    end
  end
end

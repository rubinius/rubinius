require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Rubinius::Type.object_respond_to_ary?" do
  it "returns true if #to_ary is public" do
    Rubinius::Type.object_respond_to_ary?(TypeSpecs::PublicToAry.new).should be_true
  end

  ruby_version_is "" ... "1.9" do
    it "returns false if #to_ary is private" do
      Rubinius::Type.object_respond_to_ary?(TypeSpecs::PrivateToAry.new).should be_false
    end
  end

  ruby_version_is "1.9" do
    it "returns true if #to_ary is private" do
      Rubinius::Type.object_respond_to_ary?(TypeSpecs::PrivateToAry.new).should be_true
    end
  end
end

require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Rubinius::Type.object_respond_to__dump?" do
  it "returns true if #_dump is public" do
    Rubinius::Type.object_respond_to__dump?(TypeSpecs::PublicMarshalMethods.new).should be_true
  end

  ruby_version_is "" ... "2.0" do
    it "returns false if #_dump is private" do
      Rubinius::Type.object_respond_to__dump?(TypeSpecs::PrivateMarshalMethods.new).should be_false
    end
  end

  ruby_version_is "2.0" do
    it "returns true if #_dump is private" do
      Rubinius::Type.object_respond_to__dump?(TypeSpecs::PrivateMarshalMethods.new).should be_true
    end
  end
end

require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Rubinius::Type.object_respond_to_marshal_dump?" do
  it "returns true if #marshal_dump is public" do
    Rubinius::Type.object_respond_to_marshal_dump?(TypeSpecs::PublicMarshalMethods.new).should be_true
  end

  ruby_version_is "" ... "2.0" do
    it "returns false if #marshal_dump is private" do
      Rubinius::Type.object_respond_to_marshal_dump?(TypeSpecs::PrivateMarshalMethods.new).should be_false
    end
  end

  ruby_version_is "2.0" do
    it "returns true if #marshal_dump is private" do
      Rubinius::Type.object_respond_to_marshal_dump?(TypeSpecs::PrivateMarshalMethods.new).should be_true
    end
  end
end

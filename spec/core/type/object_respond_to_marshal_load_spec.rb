require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Rubinius::Type.object_respond_to_marshal_load?" do
  it "returns true if #marshal_load is public" do
    Rubinius::Type.object_respond_to_marshal_load?(TypeSpecs::PublicMarshalMethods.new).should be_true
  end

  ruby_version_is "" ... "2.0" do
    it "returns false if #marshal_load is private" do
      Rubinius::Type.object_respond_to_marshal_load?(TypeSpecs::PrivateMarshalMethods.new).should be_false
    end
  end

  ruby_version_is "2.0" do
    it "returns true if #marshal_load is private" do
      Rubinius::Type.object_respond_to_marshal_load?(TypeSpecs::PrivateMarshalMethods.new).should be_true
    end
  end
end

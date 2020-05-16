require File.expand_path('../../../spec_helper', __FILE__)

with_feature :encoding do
  describe "Encoding.default_internal" do
    before(:each) do
      @original_encoding = Encoding.default_internal
    end

    after(:each) do
      Encoding.default_internal = @original_encoding
    end

    it "is nil" do
      Encoding.default_internal.should be_nil
    end

    it "returns nil regardless of what is assigned to it" do
      Encoding.default_internal = Encoding::ASCII
      Encoding.default_internal.should be_nil
    end
  end
end

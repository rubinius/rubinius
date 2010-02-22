require File.expand_path('../../../spec_helper', __FILE__)

describe "ENV.select" do

  ruby_version_is ""..."1.9" do
    it "returns the Hash for which block return true" do
      ENV["foo"] = "bar"
      ENV.select { |k, v| k == "foo" }.should == [["foo", "bar"]]
      ENV.delete "foo"
    end
  end

  ruby_version_is "1.9" do
    it "returns the Hash for which block return true" do
      ENV["foo"] = "bar"
      ENV.select { |k, v| k == "foo" }.should == {"foo" => "bar"}
      ENV.delete "foo"
    end
  end

  ruby_version_is "" ... "1.8.7" do
    it "raises when no block is given" do
      lambda { ENV.select }.should raise_error(LocalJumpError)
    end
  end

  ruby_version_is "1.8.7" do
    it "returns an Enumerator when no block is given" do
      ENV.select.should be_an_instance_of(enumerator_class)
    end
  end
end

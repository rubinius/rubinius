require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/methods', __FILE__)

describe "Time#zone" do
  it "returns the time zone used for time" do
    # Testing with Asia/Kuwait here because it doesn't have DST.
    with_timezone("Asia/Kuwait") do
      Time.now.zone.should == "AST"
    end
  end

  ruby_version_is "1.9" do
    it "returns nil for a Time with a fixed offset" do
      Time.new(2001, 1, 1, 0, 0, 0, "+05:00").zone.should == nil
    end

    it "returns the correct timezone for a local time" do
      t = Time.new(2005, 2, 27, 22, 50, 0, -3600)

      with_timezone("US/Eastern") do
        t.getlocal.zone.should == "EST"
      end
    end

    it "returns nil when getting the local time with a fixed offset" do
      t = Time.new(2005, 2, 27, 22, 50, 0, -3600)

      with_timezone("US/Eastern") do
        t.getlocal("+05:00").zone.should be_nil
      end
    end
  end

  it "returns UTC when called on a UTC time" do
    Time.now.utc.zone.should == "UTC"
  end
end

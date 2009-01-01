require File.dirname(__FILE__) + '/../../../spec_helper'

describe "File::Stat#rdev_major" do
  it "returns the major part of File::Stat#rdev" do
    File.stat('/dev/null').rdev_major.should be_kind_of(Integer)
  end
end

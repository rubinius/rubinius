require File.dirname(__FILE__) + '/../../../spec_helper'

describe "File::Stat#rdev_minor" do
  it "returns the minor part of File::Stat#rdev" do
    File.stat('/dev/null').rdev_minor.should be_kind_of(Integer)
  end
end

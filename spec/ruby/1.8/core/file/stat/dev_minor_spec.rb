require File.dirname(__FILE__) + '/../../../spec_helper'

describe "File::Stat.dev_minor" do
  it "returns the minor part of File::Stat#dev" do
    File.stat('/dev/null').dev_minor.should be_kind_of(Integer)
  end
end

describe "File::Stat#dev_minor" do
  it "needs to be reviewed for spec completeness" do
  end
end

require File.dirname(__FILE__) + '/../../../spec_helper'

describe "File::Stat#nlink" do
  before :each do
    @file = tmp("stat_nlink")
    @link = @file + ".lnk"
    touch @file
  end

  after :each do
    rm_r @link, @file
  end

  it "returns the number of links to a file" do
    File::Stat.new(@file).nlink.should == 1
    File.link(@file, @link)
    File::Stat.new(@file).nlink.should == 2
  end
end

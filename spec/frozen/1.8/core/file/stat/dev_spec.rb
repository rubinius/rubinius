require File.dirname(__FILE__) + '/../../../spec_helper'

describe 'File::Stat#dev' do
  it "returns the number of the device on which the file exists" do
    File.stat('/dev/null').dev.should be_kind_of(Integer)
  end
end

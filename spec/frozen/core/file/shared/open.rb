require File.dirname(__FILE__) + '/../../dir/fixtures/common'

describe :open_directory, :shared => true do
  it "opens directories" do
    File.send(@method, tmp("")).should be_kind_of(File)
  end
end

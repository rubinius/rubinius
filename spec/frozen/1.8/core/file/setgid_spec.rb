require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../shared/file/setgid'

describe "File.setgid?" do
  it_behaves_like :file_setgid, :setgid?, File
end

describe "File.setgid?" do
  it "needs to be reviewed for spec completeness" do
  end
end

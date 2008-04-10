require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../shared/file/setuid'

describe "File.setuid?" do
  it_behaves_like :file_setuid, :setuid?, File
end

describe "File.setuid?" do
  it "needs to be reviewed for spec completeness" do
  end
end

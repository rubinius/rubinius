require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/stat'

describe "File.lstat" do
  it_behaves_like :file_stat, :lstat
end

describe "File#lstat" do
  it "needs to be reviewed for spec completeness" do
  end
end

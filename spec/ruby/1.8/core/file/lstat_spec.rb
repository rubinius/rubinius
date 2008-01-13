require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/stat'

describe "File.stat" do
  it_behaves_like :file_stat, :stat
end

require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../../../shared/file/setuid'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "File::Stat#setuid?" do
  it_behaves_like :file_setuid, :setuid?, FileStat
end

describe "File::Stat#setuid?" do
  it "needs to be reviewed for spec completeness"
end

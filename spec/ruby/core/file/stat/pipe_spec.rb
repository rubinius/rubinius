require File.expand_path('../../../../spec_helper', __FILE__)
require File.expand_path('../../../../shared/file/pipe', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "File::Stat#pipe?" do
  it_behaves_like :file_pipe, :pipe?, FileStat
end

describe "File::Stat#pipe?" do
  it "needs to be reviewed for spec completeness"
end

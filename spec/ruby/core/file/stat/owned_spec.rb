require File.expand_path('../../../../spec_helper', __FILE__)
require File.expand_path('../../../../shared/file/owned', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "File::Stat#owned?" do
  it_behaves_like :file_owned, :owned?, FileStat
end

describe "File::Stat#owned?" do
  it "needs to be reviewed for spec completeness"
end

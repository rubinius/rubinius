require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../shared/file/owned'

describe "FileTest.owned?" do
  it_behaves_like :file_owned, :owned?, FileTest
end

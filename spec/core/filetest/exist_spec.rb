require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/exist'

describe "FileTest.exist?" do
  it_behaves_like(@filetest_exist, :exist?)
end

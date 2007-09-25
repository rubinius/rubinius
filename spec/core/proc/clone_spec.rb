require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/dup_spec'

describe "Proc#clone" do
  it_behaves_like(@proc_dup, :clone)
end
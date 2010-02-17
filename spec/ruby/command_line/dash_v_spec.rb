require File.dirname(__FILE__) + '/../spec_helper'
require File.dirname(__FILE__) + '/shared/verbose'

describe "The -v command line option" do
  it_behaves_like "sets $VERBOSE to true", "-v"
end

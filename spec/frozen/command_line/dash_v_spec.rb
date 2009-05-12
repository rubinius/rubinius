require File.dirname(__FILE__) + '/../spec_helper'

describe "The -v command line option" do
  it_behaves_like "sets $VERBOSE to true", "-v"
end

require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/windows'
require 'etc'

describe "Etc.group" do
  it_behaves_like(:etc_on_windows, :group)
end

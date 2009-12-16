require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/rfc2822'
require 'time'

describe "Time.rfc822" do
  it_behaves_like :time_rfc2822, :rfc822
end

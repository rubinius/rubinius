require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/reopen'
require 'syslog'

describe "Syslog.reopen" do
  it_behaves_like :syslog_reopen, :reopen
end

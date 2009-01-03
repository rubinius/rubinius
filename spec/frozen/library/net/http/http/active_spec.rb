require File.dirname(__FILE__) + '/../../../../spec_helper'
require 'net/http'
require File.dirname(__FILE__) + '/fixtures/http_server'
require File.dirname(__FILE__) + '/shared/started'

describe "Net::HTTP#active?" do
  it_behaves_like :net_http_started_p, :active?
end

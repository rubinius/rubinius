require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/xmlschema'
require 'time'

describe "Time.xmlschema" do
  it_behaves_like :time_xmlschema, :iso8601
end

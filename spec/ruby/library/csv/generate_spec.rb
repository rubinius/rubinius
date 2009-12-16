require File.dirname(__FILE__) + '/../../spec_helper'
require 'csv'
require 'tempfile'

describe "CSV.generate" do
  
  before :each do
    @outfile_name = tmp("generate_test_#{$$}.csv")  
  end
 
  it "creates a BasicWriter" do
    writer = CSV::generate(@outfile_name)
    writer.should be_kind_of(CSV::BasicWriter)
    writer.close
  end

  it "accepts a field separator" do
    writer = CSV::generate(@outfile_name, ",")
    writer.should be_kind_of(CSV::BasicWriter)
    writer.close
  end

  it "accepts a row separator" do
    writer = CSV::generate(@outfile_name, ".")
    writer.should be_kind_of(CSV::BasicWriter)
    writer.close
  end
  
  it "creates a BasicWriter to use in a block" do
    CSV::generate(@outfile_name) do |writer|
      writer.should be_kind_of(CSV::BasicWriter)
    end
  end

  it "creates a BasicWriter with ; as the separator inside the block" do
    CSV::generate(@outfile_name, ?;) do |writer|
      writer.should be_kind_of(CSV::BasicWriter)
    end
  end

  after :each do
    File.unlink(@outfile_name)
  end
end

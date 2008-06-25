require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/common'
require File.dirname(__FILE__) + '/fixtures/strings'

shared :each_document do
  describe "YAML#each_document" do
    it "calls the block on each succesive document" do
      documents = []
      YAML.each_document($multidocument) do |doc|
        documents << doc
      end
      documents.should == [["Mark McGwire", "Sammy Sosa", "Ken Griffey"], 
                           ["Chicago Cubs", "St Louis Cardinals"]]
    end

    it "works on files" do
      File.open($test_parse_file, "r") do |file|
        YAML.each_document(file) do |doc|
          doc.should == {"project"=>{"name"=>"RubySpec"}}
        end
      end
    end
  end
end

require File.dirname(__FILE__) + '/../spec_helper'
#Class methods:

#parser
#parse_documents
#add_private_type
#dump_stream
#each_document
#object_maker
#load_stream
#tagged_classes
#try_implicit
#load_documents
#load
#resolver
#detect_implicit
#parse
#add_domain_type
#read_type_class
#add_builtin_type
#dump
#generic_parser
#quick_emit
#tag_class
#tagurize
#each_node
#add_ruby_type
#load_file
#emitter
#transfer
#parse_file
require 'yaml'
$test_file = "/tmp/yaml_test.yml"

context "YAML class method" do
  specify "dump should convert object to YAML and write result to io when io provided" do
    File.open($test_file, 'w' ) do |io|
      YAML.dump( ['badger', 'elephant', 'tiger'], io )
    end
    YAML.load_file($test_file).should == ['badger', 'elephant', 'tiger']
    system "rm #{$test_file}"
  end
  
  specify "dump should return string containing dumped YAML when no io provided" do
    YAML.dump( :locked ) == "--- :locked"
  end
  
  specify "load should return document from current io stream when io provided" do
    File.open($test_file, 'w') do |io|
      YAML.dump( ['badger', 'elephant', 'tiger'], io )
    end
    File.open($test_file) { |yf| YAML::load( yf ) }.should == ['badger', 'elephant', 'tiger']
    system "rm #{$test_file}"
  end
  
  specify "load should be able to take string argument" do
    YAML.load( "--- :locked" ).should == :locked
  end
  
  specify "load_file should return hash" do
    File.open($test_file,'w' ){|io| YAML.dump( {"bar"=>2, "car"=>1}, io ) }
    YAML.load_file($test_file).should == {"bar"=>2, "car"=>1}
    system "rm #{$test_file}"
  end
  
  specify "tagged_classes should return complete dictionary of taguris paied with classes" do
    YAML.tagged_classes["tag:yaml.org,2002:int"].should == Integer
  end
  
  specify "tagurize should convert a type_id to a taguri" do
    YAML.tagurize('wtf').should == "tag:yaml.org,2002:wtf"
    YAML.tagurize(1).should == 1
  end
  
  specify "tag_class should associate taguri tag with ruby class" do
    YAML.tag_class('rubini.us','rubinius').should == "rubinius"
  end
end

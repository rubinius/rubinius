require File.dirname(__FILE__) + '/../spec_helper'


describe "The --no-rbc command line option" do
  it "won't build an rbc file when calling a single file" do
    ruby_exe "fixtures/awesome.rb", :options => "--no-rbc", :dir => File.dirname(__FILE__)
    
    File.exist?(File.join(File.dirname(__FILE__), 'fixtures/awesome.rbc')).should == false
  end
  
  it "won't build any rbc files when calling a chain of files" do
    ruby_exe "fixtures/multi_files.rb", :options => "--no-rbc", :dir => File.dirname(__FILE__)
    
    ['fixtures/multi_files.rbc', 'fixtures/required_file.rbc'].each do |f|
      File.exist?(File.join(File.dirname(__FILE__), f)).should == false
    end
  end
end

require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

$LOAD_PATH << (File.dirname(__FILE__) + '/fixtures/require')
$LOAD_PATH << (File.dirname(__FILE__) + '/fixtures/require/require_spec_rba.rba')

require 'tmpdir'

describe "Kernel#require" do
  it "loads a .rb by looking in $LOAD_PATH, only once" do
    require('require_spec_1').should == true
    $require_spec_1.should == :yep
    
    require('require_spec_1').should == false
    $LOADED_FEATURES.include?('require_spec_1.rb').should == true
  end

  it "checks $LOADED_FEATURES before the filesystem" do
    require('require_spec_6').should == true

    Dir.chdir '..' do
      require('require_spec_6').should == false
    end
  end

  compliant :rbx do
  
    it "loads a .rbc from a .rba in $LOAD_PATH, only once" do
      require('require_spec_2').should == true
      $require_spec_2.should == :yep

      require('require_spec_2').should == false
      $LOADED_FEATURES.include?('require_spec_2.rb').should == true
    end

    it "loads a .rbc even if the .rb is missing" do
      require('require_spec_3').should == true
      $require_spec_3.should == :yep

      require('require_spec_3').should == false

      $LOADED_FEATURES.include?('require_spec_3.rb').should == true
    end

    it "loads a .rbc file if it's newer than the associated .rb file" do
      path = File.expand_path(
               File.dirname(__FILE__) + '/fixtures/require/require_spec_5.rbc')
               
      # 'touch' the file so it's mod time is newer
      File.open(path,"w").close
      
      require('require_spec_5').should == true
      $require_spec_5.should == :rbc

      require('require_spec_5').should == false
    
      $LOADED_FEATURES.include?('require_spec_5.rb').should == true
    end
    
  end
  
  it "loads a file from an absolute path" do
    path = File.expand_path(
             File.dirname(__FILE__) + '/fixtures/require/require_spec_4.rb')

    require(path).should == true
    $require_spec_4.should == :yep
    
    require(path).should == false

    $LOADED_FEATURES.include?(path).should == true    
  end
  
  it "raises a LoadError if the file can't be found" do
    should_raise(LoadError) do
      require('not_around_at_all')
    end
  end

  it "only accepts strings" do
    should_raise(TypeError) { require(nil) }
    should_raise(TypeError) { require(42) }
    should_raise(TypeError) { require([]) }
  end
end

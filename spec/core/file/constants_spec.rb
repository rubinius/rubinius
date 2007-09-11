require File.dirname(__FILE__) + '/../../spec_helper'

describe "File::Constants" do 
  specify "match mode constants" do 
    File::FNM_NOESCAPE.should_not == nil
    File::FNM_PATHNAME.should_not == nil
    File::FNM_DOTMATCH.should_not == nil
    File::FNM_CASEFOLD.should_not == nil
    File::FNM_SYSCASE.should_not == nil

    platform :mswin do #|| VMS
      File::FNM_SYSCASE.should == 8
    end
  end

  # Only these constants are not inherited from the IO class
  specify "the separator constant" do
    File::SEPARATOR.should_not == nil 
    File::Separator.should_not == nil
    File::PATH_SEPARATOR.should_not == nil
    File::SEPARATOR.should == "/"

    platform :mswin do #|| VMS
      File::ALT_SEPARATOR.should_not == nil
      File::PATH_SEPARATOR.should == ";"
    end
    
    platform :not, :mswin do
      File::ALT_SEPARATOR.should == nil
      File::PATH_SEPARATOR.should == ":"
    end
  end

  specify "the open mode constants" do   
    File::APPEND.should_not == nil
    File::CREAT.should_not == nil
    File::EXCL.should_not == nil
    File::NONBLOCK.should_not == nil
    File::RDONLY.should_not == nil
    File::RDWR.should_not == nil
    File::TRUNC.should_not == nil
    File::WRONLY.should_not == nil
      
    platform :not, :mswin do # Not sure about VMS here
      File::NOCTTY.should_not == nil
    end
  end

  specify "lock mode constants" do 
    File::LOCK_EX.should_not == nil
    File::LOCK_NB.should_not == nil
    File::LOCK_SH.should_not == nil
    File::LOCK_UN.should_not == nil
  end
end

describe "File::Constants" do  
  # These mode and permission bits are platform dependent
  specify "File::RDONLY" do 
    defined?(File::RDONLY).should == "constant" 
  end
 
  specify "File::WRONLY" do  
    defined?(File::WRONLY).should == "constant" 
  end
 
  specify "File::CREAT" do     
    defined?(File::CREAT).should == "constant" 
  end
 
  specify "File::RDWR" do  
    defined?(File::RDWR).should == "constant" 
  end
 
  specify "File::APPEND" do      
    defined?(File::APPEND).should == "constant" 
  end
 
  specify "File::TRUNC" do     
    defined?(File::TRUNC).should == "constant" 
  end   
  
  platform :not, :mswin do # Not sure about VMS here
    specify "File::NOCTTY" do     
      defined?(File::NOCTTY).should == "constant" 
    end   
  end
  
  specify "File::NONBLOCK" do     
    defined?(File::NONBLOCK).should == "constant" 
  end   
  
  specify "File::LOCK_EX" do     
    defined?(File::LOCK_EX).should == "constant" 
  end   
  
  specify "File::LOCK_NB" do     
    defined?(File::LOCK_NB).should == "constant" 
  end   
  
  specify "File::LOCK_SH" do     
    defined?(File::LOCK_SH).should == "constant" 
  end   
  
  specify "File::LOCK_UN" do     
    defined?(File::LOCK_UN).should == "constant" 
  end  
  
  specify "File::SEPARATOR" do     
    defined?(File::SEPARATOR).should == "constant" 
  end  
  specify "File::Separator" do     
    defined?(File::Separator).should == "constant" 
  end  
  
  specify "File::PATH_SEPARATOR" do     
    defined?(File::PATH_SEPARATOR).should == "constant" 
  end  
  
  specify "File::SEPARATOR" do     
    defined?(File::SEPARATOR).should == "constant" 
    File::SEPARATOR.should == "/" 
  end   
  
  platform :mswin do #|| VMS 
    specify "File::ALT_SEPARATOR" do     
      defined?(File::ALT_SEPARATOR).should == "constant" 
      File::PATH_SEPARATOR.should == ";" 
    end  
  end
  
  platform :not, :mswin do
    specify "File::PATH_SEPARATOR" do     
      defined?(File::PATH_SEPARATOR).should == "constant"       
      File::PATH_SEPARATOR.should == ":"
    end  
  end
  
end

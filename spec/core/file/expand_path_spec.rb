require File.dirname(__FILE__) + '/../../spec_helper'

describe "File.expand_path" do
  before(:each) do
    platform :mswin do
      @base = `cd`.chomp.tr '\\', '/'
      @tmpdir = "c:/tmp"
      @rootdir = "c:/"
    end
    
    platform :not, :mswin do
      @base = `pwd`.chomp
      @tmpdir = "/tmp"
      @rootdir = "/"
      @pwd  = Dir.pwd
    end
  end

  after(:each) do
    @base = nil
    @tmpdir = nil
    @rootdir = nil    
    @pwd  = nil
  end
  
  it "Converts a pathname to an absolute pathname" do 
    File.expand_path('').should == @base
    File.expand_path('a').should == File.join(@base, 'a')
    File.expand_path('a', nil).should == File.join(@base, 'a')
  end
  
  it "Converts a pathname to an absolute pathname, Ruby-Talk:18512 " do 
    # Because of Ruby-Talk:18512
    File.expand_path('a.').should == File.join(@base, 'a.')
    File.expand_path('.a').should == File.join(@base, '.a')
    File.expand_path('a..').should == File.join(@base, 'a..')
    File.expand_path('..a').should == File.join(@base, '..a')
    File.expand_path('a../b').should == File.join(@base, 'a../b')
  end
  
  it "Converts a pathname to an absolute pathname, using a complete path" do     
    File.expand_path("", "#{@tmpdir}").should == "#{@tmpdir}"
    File.expand_path("a", "#{@tmpdir}").should =="#{@tmpdir}/a"
    File.expand_path("../a", "#{@tmpdir}/xxx").should == "#{@tmpdir}/a"
    File.expand_path(".", "#{@rootdir}").should == "#{@rootdir}"
  end
  
  unless not home = ENV['HOME']
    it "Converts a pathname to an absolute pathname, using ~ (home) as base" do
      File.expand_path('~').should == home
      File.expand_path('~', '/tmp/gumby/ddd').should == home
      File.expand_path('~/a', '/tmp/gumby/ddd').should == File.join(home, 'a')
    end
  end
  
  platform :not, :mswin do
    it "expand path with " do      
      File.expand_path("../../bin", "/tmp/x").should == "/bin" 
      File.expand_path("../../bin", "/tmp").should == "/bin"
      File.expand_path("../../bin", "/").should == "/bin"
      File.expand_path("../../bin", "tmp/x").should == File.join(@pwd, 'bin')
    end
  end  
  
  platform :not, :mswin do
    specify "expand_path for commoms unix path  give a full path" do      
      File.expand_path('/tmp/').should =='/tmp'
      File.expand_path('/tmp/../../../tmp').should == '/tmp'
      File.expand_path('').should == Dir.pwd
      File.expand_path('./////').should == Dir.pwd
      File.expand_path('.').should == Dir.pwd
      File.expand_path(Dir.pwd).should == Dir.pwd
      File.expand_path('~/').should == ENV['HOME']
      File.expand_path('~/..badfilename').should == ENV['HOME'] + '/..badfilename'
      File.expand_path('..').should == Dir.pwd.split('/')[0...-1].join("/")
      File.expand_path('//').should == '//'
    end
  end 
  
  it "raise an exception if the argumnents are not of the correct type or are missing" do
    should_raise(ArgumentError){ File.expand_path }
    should_raise(TypeError){ File.expand_path(1) }
    should_raise(TypeError){ File.expand_path(nil) }
    should_raise(TypeError){ File.expand_path(true) }
      
    platform :not, :mswin do
      should_raise(ArgumentError){ File.expand_path("~a_fake_file") }
    end
  end
end

require File.dirname(__FILE__) + '/../../spec_helper'

describe "File.expand_path" do
  before :each do
    platform_is :windows do
      @base = `cd`.chomp.tr '\\', '/'
      @tmpdir = "c:/tmp"
      @rootdir = "c:/"
    end

    platform_is_not :windows do
      @base = Dir.pwd
      @tmpdir = "/tmp"
      @rootdir = "/"
    end
  end

  it "converts a pathname to an absolute pathname" do
    File.expand_path('').should == @base
    File.expand_path('a').should == File.join(@base, 'a')
    File.expand_path('a', nil).should == File.join(@base, 'a')
  end

  not_compliant_on :ironruby do
    it "converts a pathname to an absolute pathname, Ruby-Talk:18512 " do
      # Because of Ruby-Talk:18512
      File.expand_path('a.').should == File.join(@base, 'a.')
      File.expand_path('.a').should == File.join(@base, '.a')
      File.expand_path('a..').should == File.join(@base, 'a..')
      File.expand_path('..a').should == File.join(@base, '..a')
      File.expand_path('a../b').should == File.join(@base, 'a../b')
    end
  end

  it "converts a pathname to an absolute pathname, using a complete path" do
    File.expand_path("", "#{@tmpdir}").should == "#{@tmpdir}"
    File.expand_path("a", "#{@tmpdir}").should =="#{@tmpdir}/a"
    File.expand_path("../a", "#{@tmpdir}/xxx").should == "#{@tmpdir}/a"
    File.expand_path(".", "#{@rootdir}").should == "#{@rootdir}"
  end

  # FIXME: do not use conditionals like this around #it blocks
  unless not home = ENV['HOME']
    it "converts a pathname to an absolute pathname, using ~ (home) as base" do
      File.expand_path('~').should == home
      File.expand_path('~', '/tmp/gumby/ddd').should == home
      File.expand_path('~/a', '/tmp/gumby/ddd').should == File.join(home, 'a')
    end
  end

  platform_is_not :windows do
    # FIXME: these are insane!
    it "expand path with " do
      File.expand_path("../../bin", "/tmp/x").should == "/bin"
      File.expand_path("../../bin", "/tmp").should == "/bin"
      File.expand_path("../../bin", "/").should == "/bin"
      File.expand_path("../../bin", "tmp/x").should == File.join(@base, 'bin')
    end

    it "expand_path for commoms unix path  give a full path" do
      File.expand_path('/tmp/').should =='/tmp'
      File.expand_path('/tmp/../../../tmp').should == '/tmp'
      File.expand_path('').should == Dir.pwd
      File.expand_path('./////').should == Dir.pwd
      File.expand_path('.').should == Dir.pwd
      File.expand_path(Dir.pwd).should == Dir.pwd
      File.expand_path('~/').should == ENV['HOME']
      File.expand_path('~/..badfilename').should == "#{ENV['HOME']}/..badfilename"
      File.expand_path('..').should == Dir.pwd.split('/')[0...-1].join("/")
      File.expand_path('//').should == '//'
      File.expand_path('~/a','~/b').should == "#{ENV['HOME']}/a"
    end

    it "raises an ArgumentError if the path is not valid" do
      lambda { File.expand_path("~a_fake_file") }.should raise_error(ArgumentError)
    end

    it "expands ~ENV['USER'] to the user's home directory" do
      File.expand_path("~#{ENV['USER']}").should == ENV['HOME']
      File.expand_path("~#{ENV['USER']}/a").should == "#{ENV['HOME']}/a"
    end
  end

  it "raises an ArgumentError is not passed one or two arguments" do
    lambda { File.expand_path }.should raise_error(ArgumentError)
    lambda { File.expand_path '../', 'tmp', 'foo' }.should raise_error(ArgumentError)
  end

  it "raises a TypeError if not passed a String type" do
    lambda { File.expand_path(1)    }.should raise_error(TypeError)
    lambda { File.expand_path(nil)  }.should raise_error(TypeError)
    lambda { File.expand_path(true) }.should raise_error(TypeError)
  end

  it "expands /./dir to /dir" do
    File.expand_path("/./dir").should == "/dir"
  end
end

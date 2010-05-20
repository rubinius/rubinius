require File.expand_path('../../../spec_helper', __FILE__)

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
    platform_is_not :windows do
      it "converts a pathname to an absolute pathname, using ~ (home) as base" do
        File.expand_path('~').should == home
        File.expand_path('~', '/tmp/gumby/ddd').should == home
        File.expand_path('~/a', '/tmp/gumby/ddd').should == File.join(home, 'a')
      end
    end
    platform_is :windows do
      it "converts a pathname to an absolute pathname, using ~ (home) as base" do
        File.expand_path('~').should == home.tr("\\", '/')
        File.expand_path('~', '/tmp/gumby/ddd').should == home.tr("\\", '/')
        File.expand_path('~/a', '/tmp/gumby/ddd').should == File.join(home.tr("\\", '/'), 'a')
      end
    end
  end

  platform_is_not :windows do
    # FIXME: these are insane!
    it "expand path with " do
      File.expand_path("../../bin", "/tmp/x").should == "/bin"
      File.expand_path("../../bin", "/tmp").should == "/bin"
      File.expand_path("../../bin", "/").should == "/bin"
      File.expand_path("../bin", "tmp/x").should == File.join(@base, 'tmp', 'bin')
      File.expand_path("../bin", "x/../tmp").should == File.join(@base, 'bin')
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
      File.expand_path('~/a','~/b').should == "#{ENV['HOME']}/a"
    end

    not_compliant_on :rubinius, :macruby do
      it "does not replace multiple '/' at the beginning of the path" do
        File.expand_path('////some/path').should == "////some/path"
      end
    end

    deviates_on :rubinius, :macruby do
      it "replaces multiple '/' with a single '/' at the beginning of the path" do
        File.expand_path('////some/path').should == "/some/path"
      end
    end

    it "replaces multiple '/' with a single '/'" do
      File.expand_path('/some////path').should == "/some/path"
    end

    it "raises an ArgumentError if the path is not valid" do
      lambda { File.expand_path("~a_not_existing_user") }.should raise_error(ArgumentError)
    end

    it "expands ~ENV['USER'] to the user's home directory" do
      File.expand_path("~#{ENV['USER']}").should == ENV['HOME']
      File.expand_path("~#{ENV['USER']}/a").should == "#{ENV['HOME']}/a"
    end

    it "does not expand ~ENV['USER'] when it's not at the start" do
      File.expand_path("/~#{ENV['USER']}/a").should == "/~#{ENV['USER']}/a"
    end

    it "expands ../foo with ~/dir as base dir to /path/to/user/home/foo" do
      File.expand_path('../foo', '~/dir').should == "#{ENV['HOME']}/foo"
    end
  end

  it "raises an ArgumentError if not passed one or two arguments" do
    lambda { File.expand_path }.should raise_error(ArgumentError)
    lambda { File.expand_path '../', 'tmp', 'foo' }.should raise_error(ArgumentError)
  end

  ruby_version_is "1.9" do
    it "accepts objects that have a #to_path method" do
      File.expand_path(mock_to_path("a"), mock_to_path("#{@tmpdir}"))
    end
  end

  it "raises a TypeError if not passed a String type" do
    lambda { File.expand_path(1)    }.should raise_error(TypeError)
    lambda { File.expand_path(nil)  }.should raise_error(TypeError)
    lambda { File.expand_path(true) }.should raise_error(TypeError)
  end

  platform_is_not :windows do
    it "expands /./dir to /dir" do
      File.expand_path("/./dir").should == "/dir"
    end
  end

  platform_is :windows do
    it "expands C:/./dir to C:/dir" do
      File.expand_path("C:/./dir").should == "C:/dir"
    end
  end

  ruby_version_is "1.9" do
    it "produces a String in the default external encoding" do
      old_external = Encoding.default_external
      Encoding.default_external = Encoding::SHIFT_JIS
      File.expand_path("./a").encoding.should == Encoding::SHIFT_JIS
      Encoding.default_external = old_external
    end
  end
end

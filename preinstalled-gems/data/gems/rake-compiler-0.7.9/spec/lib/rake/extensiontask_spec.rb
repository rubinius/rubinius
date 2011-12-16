require File.expand_path(File.dirname(__FILE__) + '/../../spec_helper')

require 'rake/extensiontask'
require 'rbconfig'

describe Rake::ExtensionTask do
  context '#new' do
    context '(basic)' do
      it 'should raise an error if no name is provided' do
        lambda {
          Rake::ExtensionTask.new
        }.should raise_error(RuntimeError, /Extension name must be provided/)
      end

      it 'should allow string as extension name assignation' do
        ext = Rake::ExtensionTask.new('extension_one')
        ext.name.should == 'extension_one'
      end

      it 'should allow string as extension name using block assignation' do
        ext = Rake::ExtensionTask.new do |ext|
          ext.name = 'extension_two'
        end
        ext.name.should == 'extension_two'
      end

      it 'should return itself for the block' do
        from_block = nil
        from_lasgn = Rake::ExtensionTask.new('extension_three') do |ext|
          from_block = ext
        end
        from_block.should == from_lasgn
      end

      it 'should accept a gem specification as parameter' do
        spec = mock_gem_spec
        ext = Rake::ExtensionTask.new('extension_three', spec)
        ext.gem_spec.should == spec
      end

      it 'should allow gem specification be defined using block assignation' do
        spec = mock_gem_spec
        ext = Rake::ExtensionTask.new('extension_four') do |ext|
          ext.gem_spec = spec
        end
        ext.gem_spec.should == spec
      end

      it 'should allow forcing of platform' do
        ext = Rake::ExtensionTask.new('weird_extension') do |ext|
          ext.platform = 'universal-foo-bar-10.5'
        end
        ext.platform.should == 'universal-foo-bar-10.5'
      end
    end
  end

  context '(defaults)' do
    before :each do
      @ext = Rake::ExtensionTask.new('extension_one')
    end

    it 'should look for extconf script' do
      @ext.config_script.should == 'extconf.rb'
    end

    it 'should dump intermediate files to tmp/' do
      @ext.tmp_dir.should == 'tmp'
    end

    it 'should copy build extension into lib/' do
      @ext.lib_dir.should == 'lib'
    end

    it 'should look for C files pattern (.c)' do
      @ext.source_pattern.should == "*.c"
    end

    it 'should have no configuration options preset to delegate' do
      @ext.config_options.should be_empty
    end

    it "should have no includes preset to delegate" do
      @ext.config_includes.should be_empty
    end

    it 'should default to current platform' do
      @ext.platform.should == RUBY_PLATFORM
    end

    it 'should default to no cross compilation' do
      @ext.cross_compile.should be_false
    end

    it 'should have no configuration options for cross compilation' do
      @ext.cross_config_options.should be_empty
    end

    it "should have cross platform defined to 'i386-mingw32'" do
      @ext.cross_platform.should == 'i386-mingw32'
    end
  end

  context '(tasks)' do
    before :each do
      Rake.application.clear
      CLEAN.clear
      CLOBBER.clear
    end

    context '(one extension)' do
      before :each do
        Rake::FileList.stub!(:[]).and_return(["ext/extension_one/source.c"], [])
        @ext = Rake::ExtensionTask.new('extension_one')
        @ext_bin = ext_bin('extension_one')
        @platform = RUBY_PLATFORM
        @ruby_ver = RUBY_VERSION
      end

      context 'compile' do
        it 'should define as task' do
          Rake::Task.task_defined?('compile').should be_true
        end

        it "should depend on 'compile:{platform}'" do
          Rake::Task['compile'].prerequisites.should include("compile:#{@platform}")
        end
      end

      context 'compile:extension_one' do
        it 'should define as task' do
          Rake::Task.task_defined?('compile:extension_one').should be_true
        end

        it "should depend on 'compile:extension_one:{platform}'" do
          Rake::Task['compile:extension_one'].prerequisites.should include("compile:extension_one:#{@platform}")
        end
      end

      context 'lib/extension_one.{so,bundle}' do
        it 'should define as task' do
          Rake::Task.task_defined?("lib/#{@ext_bin}").should be_true
        end

        it "should depend on 'copy:extension_one:{platform}:{ruby_ver}'" do
          Rake::Task["lib/#{@ext_bin}"].prerequisites.should include("copy:extension_one:#{@platform}:#{@ruby_ver}")
        end
      end

      context 'tmp/{platform}/extension_one/{ruby_ver}/extension_one.{so,bundle}' do
        it 'should define as task' do
          Rake::Task.task_defined?("tmp/#{@platform}/extension_one/#{@ruby_ver}/#{@ext_bin}").should be_true
        end

        it "should depend on 'tmp/{platform}/extension_one/{ruby_ver}/Makefile'" do
          Rake::Task["tmp/#{@platform}/extension_one/#{@ruby_ver}/#{@ext_bin}"].prerequisites.should include("tmp/#{@platform}/extension_one/#{@ruby_ver}/Makefile")
        end

        it "should depend on 'ext/extension_one/source.c'" do
          Rake::Task["tmp/#{@platform}/extension_one/#{@ruby_ver}/#{@ext_bin}"].prerequisites.should include("ext/extension_one/source.c")
        end

        it "should not depend on 'ext/extension_one/source.h'" do
          Rake::Task["tmp/#{@platform}/extension_one/#{@ruby_ver}/#{@ext_bin}"].prerequisites.should_not include("ext/extension_one/source.h")
        end
      end

      context 'tmp/{platform}/extension_one/{ruby_ver}/Makefile' do
        it 'should define as task' do
          Rake::Task.task_defined?("tmp/#{@platform}/extension_one/#{@ruby_ver}/Makefile").should be_true
        end

        it "should depend on 'tmp/{platform}/extension_one/{ruby_ver}'" do
          Rake::Task["tmp/#{@platform}/extension_one/#{@ruby_ver}/Makefile"].prerequisites.should include("tmp/#{@platform}/extension_one/#{@ruby_ver}")
        end

        it "should depend on 'ext/extension_one/extconf.rb'" do
          Rake::Task["tmp/#{@platform}/extension_one/#{@ruby_ver}/Makefile"].prerequisites.should include("ext/extension_one/extconf.rb")
        end
      end

      context 'clean' do
        it "should include 'tmp/{platform}/extension_one/{ruby_ver}' in the pattern" do
          CLEAN.should include("tmp/#{@platform}/extension_one/#{@ruby_ver}")
        end
      end

      context 'clobber' do
        it "should include 'lib/extension_one.{so,bundle}'" do
          CLOBBER.should include("lib/#{@ext_bin}")
        end

        it "should include 'tmp'" do
          CLOBBER.should include('tmp')
        end
      end

      it "should warn when pre-compiled files exist in extension directory" do
        Rake::FileList.stub!(:[]).
          and_return(["ext/extension_one/source.c"],
                      ["ext/extension_one/source.o"])

        _, err = capture_output do
          Rake::ExtensionTask.new('extension_one')
        end
        err.should match(/rake-compiler found compiled files in 'ext\/extension_one' directory. Please remove them./)
      end
    end

    context '(extension in custom location)' do
      before :each do
        Rake::FileList.stub!(:[]).and_return(["ext/extension_one/source.c"], [])
        @ext = Rake::ExtensionTask.new('extension_one') do |ext|
          ext.ext_dir = 'custom/ext/foo'
        end
        @ext_bin = ext_bin('extension_one')
        @platform = RUBY_PLATFORM
        @ruby_ver = RUBY_VERSION
      end

      context 'tmp/{platform}/extension_one/{ruby_ver}/Makefile' do
        it "should depend on 'custom/ext/foo/extconf.rb'" do
          Rake::Task["tmp/#{@platform}/extension_one/#{@ruby_ver}/Makefile"].prerequisites.should include("custom/ext/foo/extconf.rb")
        end
      end
    end

    context '(native tasks)' do
      before :each do
        Rake::FileList.stub!(:[]).and_return(["ext/extension_one/source.c"], [])
        @spec = mock_gem_spec
        @ext_bin = ext_bin('extension_one')
        @platform = RUBY_PLATFORM
        @ruby_ver = RUBY_VERSION
      end

      context 'native' do
        before :each do
          @spec.stub!(:platform=).and_return('ruby')
        end

        it 'should define a task for building the supplied gem' do
          Rake::ExtensionTask.new('extension_one', @spec)
          Rake::Task.task_defined?('native:my_gem').should be_true
        end

        it 'should define as task for pure ruby gems' do
          Rake::Task.task_defined?('native').should be_false
          Rake::ExtensionTask.new('extension_one', @spec)
          Rake::Task.task_defined?('native').should be_true
        end

        it 'should not define a task for already native gems' do
          @spec.stub!(:platform).and_return('current')
          Rake::ExtensionTask.new('extension_one', @spec)
          Rake::Task.task_defined?('native').should be_false
        end

        it 'should depend on platform specific native tasks' do
          Rake::ExtensionTask.new('extension_one', @spec)
          Rake::Task["native"].prerequisites.should include("native:#{@platform}")
        end

        context 'native:my_gem:{platform}' do
          it 'should depend on binary extension' do
            Rake::ExtensionTask.new('extension_one', @spec)
            Rake::Task["native:my_gem:#{@platform}"].prerequisites.should include("lib/#{@ext_bin}")
          end
        end
      end
    end

    context '(cross platform tasks)' do
      before :each do
        File.stub!(:exist?).and_return(true)
        YAML.stub!(:load_file).and_return(mock_config_yml)
        Rake::FileList.stub!(:[]).and_return(["ext/extension_one/source.c"], [])
        @spec = mock_gem_spec
        @config_file = File.expand_path("~/.rake-compiler/config.yml")
        @ruby_ver = RUBY_VERSION
        @config_path = mock_config_yml["rbconfig-#{@ruby_ver}"]

        File.stub!(:open).and_yield(mock_fake_rb)
      end

      it 'should warn if no rake-compiler configuration exist' do
        File.should_receive(:exist?).with(@config_file).and_return(false)

        out, err = capture_output do
          Rake::ExtensionTask.new('extension_one') do |ext|
            ext.cross_compile = true
          end
        end

        err.should match(/rake-compiler must be configured first to enable cross-compilation/)
      end

      it 'should parse the config file using YAML' do
        YAML.should_receive(:load_file).with(@config_file).and_return(mock_config_yml)
        Rake::ExtensionTask.new('extension_one') do |ext|
          ext.cross_compile = true
        end
      end

      it 'should warn if no section of config file defines running version of ruby' do
        config = mock(Hash)
        config.should_receive(:[]).with("rbconfig-#{@ruby_ver}").and_return(nil)
        YAML.stub!(:load_file).and_return(config)
        out, err = capture_output do
          Rake::ExtensionTask.new('extension_one') do |ext|
            ext.cross_compile = true
          end
        end
        err.should match(/no configuration section for specified version of Ruby/)
      end

      it 'should capture an action block to be executed when cross compiling' do
        lambda {
          Rake::ExtensionTask.new('extension_one') do |ext|
            ext.cross_compiling do |gem_spec|
              gem_spec.post_install_message = "Cross compiled gem"
            end
          end
        }.should_not raise_error
      end

      it 'should allow usage of RUBY_CC_VERSION to indicate a different version of ruby' do
        config = mock(Hash)
        config.should_receive(:[]).with("rbconfig-1.9.1").and_return('/path/to/ruby/1.9.1/rbconfig.rb')
        YAML.stub!(:load_file).and_return(config)

        ENV['RUBY_CC_VERSION'] = '1.9.1'
        Rake::ExtensionTask.new('extension_one') do |ext|
          ext.cross_compile = true
        end
      end

      it 'should allow multiple versions be supplied to RUBY_CC_VERSION' do
        config = mock(Hash)
        config.should_receive(:[]).once.with("rbconfig-1.8.6").and_return('/path/to/ruby/1.8.6/rbconfig.rb')
        config.should_receive(:[]).once.with("rbconfig-1.9.1").and_return('/path/to/ruby/1.9.1/rbconfig.rb')
        YAML.stub!(:load_file).and_return(config)

        ENV['RUBY_CC_VERSION'] = '1.8.6:1.9.1'
        Rake::ExtensionTask.new('extension_one') do |ext|
          ext.cross_compile = true
        end
      end

      after :each do
        ENV.delete('RUBY_CC_VERSION')
      end

      context "(cross compile for multiple versions)" do
        before :each do
          config = mock(Hash)
          config.stub!(:[]).and_return('/path/to/ruby/1.8.6/rbconfig.rb', '/path/to/ruby/1.9.1/rbconfig.rb')
          YAML.stub!(:load_file).and_return(config)

          ENV['RUBY_CC_VERSION'] = '1.8.6:1.9.1'
          @ext = Rake::ExtensionTask.new('extension_one') do |ext|
            ext.cross_compile = true
            ext.cross_platform = 'universal-unknown'
          end
        end

        it 'should create specific copy of binaries for each version' do
          Rake::Task.should have_defined("copy:extension_one:universal-unknown:1.8.6")
          Rake::Task.should have_defined("copy:extension_one:universal-unknown:1.9.1")
        end
      end

      context "(cross for 'universal-unknown' platform)" do
        before :each do
          @ext = Rake::ExtensionTask.new('extension_one', @spec) do |ext|
            ext.cross_compile = true
            ext.cross_platform = 'universal-unknown'
          end
        end

        context 'fake' do
          it 'should chain fake task to Makefile generation' do
            Rake::Task["tmp/universal-unknown/extension_one/#{@ruby_ver}/Makefile"].prerequisites.should include("tmp/universal-unknown/extension_one/#{@ruby_ver}/fake.rb")
          end
        end

        context 'rbconfig' do
          it 'should chain rbconfig tasks to Makefile generation' do
            Rake::Task["tmp/universal-unknown/extension_one/#{@ruby_ver}/Makefile"].prerequisites.should include("tmp/universal-unknown/extension_one/#{@ruby_ver}/rbconfig.rb")
          end

          it 'should take rbconfig from rake-compiler configuration' do
            Rake::Task["tmp/universal-unknown/extension_one/#{@ruby_ver}/rbconfig.rb"].prerequisites.should include(@config_path)
          end
        end

        context 'mkmf' do
          it 'should chain mkmf tasks to Makefile generation' do
            Rake::Task["tmp/universal-unknown/extension_one/#{@ruby_ver}/Makefile"].prerequisites.should include("tmp/universal-unknown/extension_one/#{@ruby_ver}/mkmf.rb")
          end

          it 'should take mkmf from rake-compiler configuration' do
            mkmf_path = File.expand_path(File.join(File.dirname(@config_path), '..', 'mkmf.rb'))
            Rake::Task["tmp/universal-unknown/extension_one/#{@ruby_ver}/mkmf.rb"].prerequisites.should include(mkmf_path)
          end
        end

        context 'compile:universal-unknown' do
          it "should be defined" do
            Rake::Task.task_defined?('compile:universal-unknown').should be_true
          end

          it "should depend on 'compile:extension_one:universal-unknown'" do
            Rake::Task['compile:universal-unknown'].prerequisites.should include('compile:extension_one:universal-unknown')
          end
        end

        context 'native:universal-unknown' do
          it "should be defined" do
            Rake::Task.task_defined?('native:universal-unknown').should be_true
          end

          it "should depend on 'native:my_gem:universal-unknown'" do
            Rake::Task['native:universal-unknown'].prerequisites.should include('native:my_gem:universal-unknown')
          end
        end
      end

      context '(cross for multiple platforms)' do
        it 'should define task for each supplied platform' do
          @ext = Rake::ExtensionTask.new('extension_one', @spec) do |ext|
            ext.cross_compile = true
            ext.cross_platform = ['universal-known', 'universal-unknown']
          end

          Rake::Task.should have_defined('compile:universal-known')
          Rake::Task.should have_defined('compile:universal-unknown')
        end
      end
    end
  end

  private
  def ext_bin(extension_name)
    "#{extension_name}.#{RbConfig::CONFIG['DLEXT']}"
  end

  def mock_gem_spec(stubs = {})
    mock(Gem::Specification,
      { :name => 'my_gem', :platform => 'ruby' }.merge(stubs)
    )
  end

  def mock_config_yml
    {
      'rbconfig-1.8.6' => '/some/path/version/1.8/to/rbconfig.rb',
      'rbconfig-1.8.7' => '/some/path/version/1.8/to/rbconfig.rb',
      'rbconfig-1.9.1' => '/some/path/version/1.9.1/to/rbconfig.rb',
      'rbconfig-1.9.2' => '/some/path/version/1.9.1/to/rbconfig.rb',
      'rbconfig-1.9.3' => '/some/path/version/1.9.1/to/rbconfig.rb',
      'rbconfig-3.0.0' => '/some/fake/version/3.0.0/to/rbconfig.rb'
    }
  end

  def mock_fake_rb
    mock(File, :write => 45)
  end
end

require File.dirname(__FILE__) + '/../../spec_helper.rb'

context "OptionParser" do
  setup do
    @out = StringIO.new
    @err = StringIO.new
    @parser = Spec::Runner::OptionParser.new
  end

  def parse(args)
    @parser.parse(args, @err, @out, true)
  end

  specify "should accept dry run option" do
    options = parse(["--dry-run"])
    options.dry_run.should_be(true)
  end

  specify "should eval and use custom formatter when none of the builtins" do
    options = parse(["--format", "Custom::Formatter"])
    options.formatter_type.should equal(Custom::Formatter)
  end

  specify "should not be verbose by default" do
    options = parse([])
    options.verbose.should_be(nil)
  end

  specify "should not use colour by default" do
    options = parse([])
    options.colour.should_be(nil)
  end

  specify "should print help to stdout" do
    options = parse(["--help"])
    @out.rewind
    @out.read.should_match(/Usage: spec \[options\] \(FILE\|DIRECTORY\|GLOB\)\+/n)
  end

  specify "should print instructions about how to fix bad formatter" do
    options = parse(["--format", "Custom::BadFormatter"])
    @err.string.should_match(/Couldn't find formatter class Custom::BadFormatter/n)
  end

  specify "should print usage to err if no dir specified" do
    options = parse([])
    @err.string.should_match(/Usage: spec/)
  end

  specify "should print version to stdout" do
    options = parse(["--version"])
    @out.rewind
    @out.read.should_match(/RSpec-\d+\.\d+\.\d+ \(r\d+\) - BDD for Ruby\nhttp:\/\/rspec.rubyforge.org\/\n/n)
  end
  
  specify "should accept -o option" do
    options = parse(["-o", "#{File.expand_path(File.dirname(__FILE__))}/output_file.txt"])
    options.out.should_be_an_instance_of File
    options.out.path.should == "#{File.expand_path(File.dirname(__FILE__))}/output_file.txt"
    File.delete(options.out.path) rescue nil
  end

  specify "should require file when require specified" do
    lambda do
      parse(["--require", "whatever"])
    end.should_raise(LoadError)
  end

  specify "should select dry run for rdoc formatter" do
    options = parse(["--format", "rdoc"])
    options.dry_run.should_be(true)
  end

  specify "should support c option" do
    options = parse(["-c"])
    options.colour.should_be(true)
  end

  specify "should support queens colour option" do
    options = parse(["--colour"])
    options.colour.should_be(true)
  end

  specify "should support us color option" do
    options = parse(["--color"])
    options.colour.should_be(true)
  end

  specify "should support single spec with s option" do
    options = parse(["-s", "something or other"])
    options.spec_name.should_eql("something or other")
  end

  specify "should support single spec with spec option" do
    options = parse(["--spec", "something or other"])
    options.spec_name.should_eql("something or other")
  end

  specify "should use html formatter when format is h" do
    options = parse(["--format", "h"])
    options.formatter_type.should equal(Spec::Runner::Formatter::HtmlFormatter)
  end

  specify "should use html formatter when format is html" do
    options = parse(["--format", "html"])
    options.formatter_type.should equal(Spec::Runner::Formatter::HtmlFormatter)
  end

  specify "should use noisy backtrace tweaker with b option" do
    options = parse(["-b"])
    options.backtrace_tweaker.should_be_instance_of(Spec::Runner::NoisyBacktraceTweaker)
  end

  specify "should use noisy backtrace tweaker with backtrace option" do
    options = parse(["--backtrace"])
    options.backtrace_tweaker.should_be_instance_of(Spec::Runner::NoisyBacktraceTweaker)
  end

  specify "should use quiet backtrace tweaker by default" do
    options = parse([])
    options.backtrace_tweaker.should_be_instance_of(Spec::Runner::QuietBacktraceTweaker)
  end

  specify "should use progress bar formatter by default" do
    options = parse([])
    options.formatter_type.should equal(Spec::Runner::Formatter::ProgressBarFormatter)
  end

  specify "should use rdoc formatter when format is r" do
    options = parse(["--format", "r"])
    options.formatter_type.should equal(Spec::Runner::Formatter::RdocFormatter)
  end

  specify "should use rdoc formatter when format is rdoc" do
    options = parse(["--format", "rdoc"])
    options.formatter_type.should equal(Spec::Runner::Formatter::RdocFormatter)
  end

  specify "should use specdoc formatter when format is s" do
    options = parse(["--format", "s"])
    options.formatter_type.should equal(Spec::Runner::Formatter::SpecdocFormatter)
  end

  specify "should use specdoc formatter when format is specdoc" do
    options = parse(["--format", "specdoc"])
    options.formatter_type.should equal(Spec::Runner::Formatter::SpecdocFormatter)
  end

  specify "should support diff option when format is not specified" do
    options = parse(["--diff"])
    options.diff_format.should_be :unified
  end

  specify "should use unified diff format option when format is unified" do
    options = parse(["--diff", "unified"])
    options.diff_format.should_be :unified
    options.differ_class.should_be Spec::Expectations::Differs::Default
  end

  specify "should use context diff format option when format is context" do
    options = parse(["--diff", "context"])
    options.diff_format.should eql(:context)
    options.differ_class.should_eql Spec::Expectations::Differs::Default
  end

  specify "should use custom diff format option when format is a custom format" do
    options = parse(["--diff", "Custom::Formatter"])
    options.diff_format.should_be :custom
    options.differ_class.should_eql Custom::Formatter
  end

  specify "should print instructions about how to fix bad differ" do
    options = parse(["--diff", "Custom::BadFormatter"])
    @err.string.should_match(/Couldn't find differ class Custom::BadFormatter/n)
  end

  specify "should support --line to identify spec" do
    spec_parser = mock("spec_parser")
    @parser.instance_variable_set('@spec_parser', spec_parser)

    file_factory = mock("File")
    file_factory.should_receive(:file?).and_return(true)
    file_factory.should_receive(:open).and_return("fake_io")
    @parser.instance_variable_set('@file_factory', file_factory)

    spec_parser.should_receive(:spec_name_for).with("fake_io", 169).and_return("some spec")

    options = parse(["some file", "--line", "169"])
    options.spec_name.should_eql("some spec")
    File.__verify
  end

  specify "should fail with error message if file is dir along with --line" do
    spec_parser = mock("spec_parser")
    @parser.instance_variable_set('@spec_parser', spec_parser)

    file_factory = mock("File")
    file_factory.should_receive(:file?).and_return(false)
    file_factory.should_receive(:directory?).and_return(true)
    @parser.instance_variable_set('@file_factory', file_factory)

    options = parse(["some file", "--line", "169"])
    @err.string.should_match(/You must specify one file, not a directory when using the --line option/n)
  end

  specify "should fail with error message if file is dir along with --line" do
    spec_parser = mock("spec_parser")
    @parser.instance_variable_set('@spec_parser', spec_parser)

    file_factory = mock("File")
    file_factory.should_receive(:file?).and_return(false)
    file_factory.should_receive(:directory?).and_return(false)
    @parser.instance_variable_set('@file_factory', file_factory)

    options = parse(["some file", "--line", "169"])
    @err.string.should_match(/some file does not exist/n)
  end

  specify "should fail with error message if more than one files are specified along with --line" do
    spec_parser = mock("spec_parser")
    @parser.instance_variable_set('@spec_parser', spec_parser)

    options = parse(["some file", "some other file", "--line", "169"])
    @err.string.should_match(/Only one file can be specified when using the --line option/n)
  end

  specify "should fail with error message if --spec and --line are used simultaneously" do
    spec_parser = mock("spec_parser")
    @parser.instance_variable_set('@spec_parser', spec_parser)

    options = parse(["some file", "--spec", "some spec", "--line", "169"])
    @err.string.should_match(/You cannot use both --line and --spec/n)
  end

  if(PLATFORM != "i386-mswin32")
    specify "should heckle when --heckle is specified (and platform is not windows)" do
      options = parse(["--heckle", "Spec"])
      options.heckle_runner.should_be_instance_of(Spec::Runner::HeckleRunner)
    end
  else
    specify "should barf when --heckle is specified (and platform is windows)" do
      lambda do
        options = parse(["--heckle", "Spec"])
      end.should_raise(StandardError, "Heckle not supported on Windows")
    end
  end
   
  specify "should read options from file when --options is specified" do
    Spec::Runner::CommandLine.should_receive(:run).with(["--diff", "--colour"], @err, @out, true, true)
    options = parse(["--options", File.dirname(__FILE__) + "/spec.opts"])
  end

  specify "should append options from file when --options is specified" do
    Spec::Runner::CommandLine.should_receive(:run).with(["some/spec.rb", "--diff", "--colour"], @err, @out, true, true)
    options = parse(["some/spec.rb", "--options", File.dirname(__FILE__) + "/spec.opts"])
  end
   
  specify "should save config to file when --generate-options is specified" do
    FileUtils.rm 'spec.opts' rescue nil
    options = parse(["--colour", "--generate-options", "spec.opts", "--diff"])
    File.open('spec.opts').read.should == "--colour\n--diff\n"
    FileUtils.rm 'spec.opts' rescue nil
  end

  specify "should call DrbCommandLine when --drb is specified" do
    Spec::Runner::DrbCommandLine.should_receive(:run).with(["some/spec.rb", "--diff", "--colour"], @err, @out, true, true)
    options = parse(["some/spec.rb", "--diff", "--drb", "--colour"])
  end
end

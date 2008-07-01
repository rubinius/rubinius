require File.dirname(__FILE__) + '/../spec_helper'
require 'mspec/utils/options'
require 'mspec/version'
require 'mspec/runner/mspec'
require 'mspec/runner/formatters'

describe MSpecOptions, "#parser" do
  it "returns an OptionParser instance" do
    MSpecOptions.new({}, "spec").parser.should be_kind_of(OptionParser)
  end
end

describe "The -B, --config FILE option" do
  before :each do
    @options, @config = new_option
  end

  it "is enabled with #add_config { }" do
    @options.should_receive(:on).with("-B", "--config FILE",
        String, an_instance_of(String))
    @options.add_config {}
  end

  it "calls the passed block" do
    ["-B", "--config"].each do |opt|
      ScratchPad.clear

      @options.add_config { |x| ScratchPad.record x }
      @options.parse [opt, "file"]
      ScratchPad.recorded.should == "file"
    end
  end
end

describe "The -n, --name RUBY_NAME option" do
  before :each do
    @verbose, $VERBOSE = $VERBOSE, nil
    @options, @config = new_option
  end

  after :each do
    $VERBOSE = @verbose
  end

  it "is enabled with #add_name" do
    @options.should_receive(:on).with("-n", "--name RUBY_NAME",
        String, an_instance_of(String))
    @options.add_name
  end

  it "sets RUBY_NAME when invoked" do
    Object.should_receive(:const_set).with(:RUBY_NAME, "name").twice
    @options.add_name
    @options.parse ["-n", "name"]
    @options.parse ["--name", "name"]
  end
end

describe "The -t, --target TARGET option" do
  before :each do
    @options, @config = new_option
    @options.add_targets
  end

  it "is enabled with #add_targets" do
    @options.stub!(:on)
    @options.should_receive(:on).with("-t", "--target TARGET",
        String, an_instance_of(String))
    @options.add_targets
  end

  it "sets the target to 'ruby' and flags to verbose with TARGET 'ruby'" do
    ["-t", "--target"].each do |opt|
      @options.parse [opt, "ruby"]
      @config[:target].should == "ruby"
      @config[:flags].should include("-v")
    end
  end

  it "sets the target to 'ruby19' with TARGET 'r19' or 'ruby19'" do
    ["-t", "--target"].each do |opt|
      ["r19", "ruby19"].each do |t|
        @options.parse [opt, t]
        @config[:target].should == "ruby19"
      end
    end
  end

  it "sets the target to 'jruby' with TARGET 'j' or 'jruby'" do
    ["-t", "--target"].each do |opt|
      ["j", "jruby"].each do |t|
        @options.parse [opt, t]
        @config[:target].should == "jruby"
      end
    end
  end

  it "sets the target to 'shotgun/rubinius' with TARGET 'x' or 'rubinius'" do
    ["-t", "--target"].each do |opt|
      ["x", "rubinius"].each do |t|
        @options.parse [opt, t]
        @config[:target].should == "shotgun/rubinius"
      end
    end
  end

  it "set the target to 'rbx' with TARGET 'rbx'" do
    ["-t", "--target"].each do |opt|
      ["X", "rbx"].each do |t|
        @options.parse [opt, t]
        @config[:target].should == "rbx"
      end
    end
  end

  it "sets the target to TARGET" do
    ["-t", "--target"].each do |opt|
      @options.parse [opt, "whateva"]
      @config[:target].should == "whateva"
    end
  end
end

describe "The -T, --target-opt OPT option" do
  before :each do
    @options, @config = new_option
    @options.add_targets
  end

  it "is enabled with #add_targets" do
    @options.stub!(:on)
    @options.should_receive(:on).with("-T", "--target-opt OPT",
        String, an_instance_of(String))
    @options.add_targets
  end

  it "adds OPT to flags" do
    ["-T", "--target-opt"].each do |opt|
      @config[:flags].delete "--whateva"
      @options.parse [opt, "--whateva"]
      @config[:flags].should include("--whateva")
    end
  end
end

describe "The -I, --include DIR option" do
  before :each do
    @options, @config = new_option
    @options.add_targets
  end

  it "is enabled with #add_targets" do
    @options.stub!(:on)
    @options.should_receive(:on).with("-I", "--include DIR",
        String, an_instance_of(String))
    @options.add_targets
  end

  it "add DIR to the includes list" do
    ["-I", "--include"].each do |opt|
      @config[:includes].delete "-Ipackage"
      @options.parse [opt, "package"]
      @config[:includes].should include("-Ipackage")
    end
  end
end

describe "The -r, --require LIBRARY option" do
  before :each do
    @options, @config = new_option
    @options.add_targets
  end

  it "is enabled with #add_targets" do
    @options.stub!(:on)
    @options.should_receive(:on).with("-r", "--require LIBRARY",
        String, an_instance_of(String))
    @options.add_targets
  end

  it "adds LIBRARY to the requires list" do
    ["-r", "--require"].each do |opt|
      @config[:requires].delete "-rlibrick"
      @options.parse [opt, "librick"]
      @config[:requires].should include("-rlibrick")
    end
  end
end

describe "The -f, --format FORMAT option" do
  before :each do
    @options, @config = new_option
    @options.add_formatters
  end

  it "is enabled with #add_formatters" do
    @options.stub!(:on)
    @options.should_receive(:on).with("-f", "--format FORMAT",
        String, an_instance_of(String))
    @options.add_formatters
  end

  it "sets the SpecdocFormatter with FORMAT 's' or 'specdoc'" do
    ["-f", "--format"].each do |opt|
      ["s", "specdoc"].each do |f|
        @config[:formatter] = nil
        @options.parse [opt, f]
        @config[:formatter].should == SpecdocFormatter
      end
    end
  end

  it "sets the HtmlFormatter with FORMAT 'h' or 'html'" do
    ["-f", "--format"].each do |opt|
      ["h", "html"].each do |f|
        @config[:formatter] = nil
        @options.parse [opt, f]
        @config[:formatter].should == HtmlFormatter
      end
    end
  end

  it "sets the DottedFormatter with FORMAT 'd', 'dot' or 'dotted'" do
    ["-f", "--format"].each do |opt|
      ["d", "dot", "dotted"].each do |f|
        @config[:formatter] = nil
        @options.parse [opt, f]
        @config[:formatter].should == DottedFormatter
      end
    end
  end

  it "sets the UnitdiffFormatter with FORMAT 'u', 'unit', or 'unitdiff'" do
    ["-f", "--format"].each do |opt|
      ["u", "unit", "unitdiff"].each do |f|
        @config[:formatter] = nil
        @options.parse [opt, f]
        @config[:formatter].should == UnitdiffFormatter
      end
    end
  end

  it "sets the SummaryFormatter with FORMAT 'm' or 'summary'" do
    ["-f", "--format"].each do |opt|
      ["m", "summary"].each do |f|
        @config[:formatter] = nil
        @options.parse [opt, f]
        @config[:formatter].should == SummaryFormatter
      end
    end
  end

  it "sets the SpinnerFormatter with FORMAT 'a', '*', or 'spin'" do
    ["-f", "--format"].each do |opt|
      ["a", "*", "spin"].each do |f|
        @config[:formatter] = nil
        @options.parse [opt, f]
        @config[:formatter].should == SpinnerFormatter
      end
    end
  end
end

describe "The -o, --output FILE option" do
  before :each do
    @options, @config = new_option
    @options.add_formatters
  end

  it "is enabled with #add_formatters" do
    @options.stub!(:on)
    @options.should_receive(:on).with("-o", "--output FILE",
        String, an_instance_of(String))
    @options.add_formatters
  end

  it "sets the output to FILE" do
    ["-o", "--output"].each do |opt|
      @config[:output] = nil
      @options.parse [opt, "some/file"]
      @config[:output].should == "some/file"
    end
  end
end

describe "The -e, --example STR" do
  before :each do
    @options, @config = new_option
    @options.add_filters
  end

  it "is enabled with #add_filters" do
    @options.stub!(:on)
    @options.should_receive(:on).with("-e", "--example STR",
        String, an_instance_of(String))
    @options.add_filters
  end

  it "adds STR to the includes list" do
    ["-e", "--example"].each do |opt|
      @config[:includes] = []
      @options.parse [opt, "this spec"]
      @config[:includes].should include("this spec")
    end
  end
end

describe "The -E, --exclude STR" do
  before :each do
    @options, @config = new_option
    @options.add_filters
  end

  it "is enabled with #add_filters" do
    @options.stub!(:on)
    @options.should_receive(:on).with("-E", "--exclude STR",
        String, an_instance_of(String))
    @options.add_filters
  end

  it "adds STR to the excludes list" do
    ["-E", "--exclude"].each do |opt|
      @config[:excludes] = []
      @options.parse [opt, "this spec"]
      @config[:excludes].should include("this spec")
    end
  end
end

describe "The -p, --pattern PATTERN" do
  before :each do
    @options, @config = new_option
    @options.add_filters
  end

  it "is enabled with #add_filters" do
    @options.stub!(:on)
    @options.should_receive(:on).with("-p", "--pattern PATTERN",
        Regexp, an_instance_of(String))
    @options.add_filters
  end

  it "adds PATTERN to the included patterns list" do
    ["-p", "--pattern"].each do |opt|
      @config[:patterns] = []
      @options.parse [opt, "this spec"]
      @config[:patterns].should include(/this spec/)
    end
  end
end

describe "The -P, --excl-pattern PATTERN" do
  before :each do
    @options, @config = new_option
    @options.add_filters
  end

  it "is enabled with #add_filters" do
    @options.stub!(:on)
    @options.should_receive(:on).with("-P", "--excl-pattern PATTERN",
        Regexp, an_instance_of(String))
    @options.add_filters
  end

  it "adds PATTERN to the excluded patterns list" do
    ["-P", "--excl-pattern"].each do |opt|
      @config[:xpatterns] = []
      @options.parse [opt, "this spec"]
      @config[:xpatterns].should include(/this spec/)
    end
  end
end

describe "The -g, --tag TAG" do
  before :each do
    @options, @config = new_option
    @options.add_filters
  end

  it "is enabled with #add_filters" do
    @options.stub!(:on)
    @options.should_receive(:on).with("-g", "--tag TAG",
        String, an_instance_of(String))
    @options.add_filters
  end

  it "adds TAG to the included tags list" do
    ["-g", "--tag"].each do |opt|
      @config[:tags] = []
      @options.parse [opt, "this spec"]
      @config[:tags].should include("this spec")
    end
  end
end

describe "The -G, --excl-tag TAG" do
  before :each do
    @options, @config = new_option
    @options.add_filters
  end

  it "is enabled with #add_filters" do
    @options.stub!(:on)
    @options.should_receive(:on).with("-G", "--excl-tag TAG",
        String, an_instance_of(String))
    @options.add_filters
  end

  it "adds TAG to the excluded tags list" do
    ["-G", "--excl-tag"].each do |opt|
      @config[:xtags] = []
      @options.parse [opt, "this spec"]
      @config[:xtags].should include("this spec")
    end
  end
end

describe "The -w, --profile FILE option" do
  before :each do
    @options, @config = new_option
    @options.add_filters
  end

  it "is enabled with #add_filters" do
    @options.stub!(:on)
    @options.should_receive(:on).with("-w", "--profile FILE",
        String, an_instance_of(String))
    @options.add_filters
  end

  it "adds FILE to the included profiles list" do
    ["-w", "--profile"].each do |opt|
      @config[:profiles] = []
      @options.parse [opt, "spec/profiles/rails.yaml"]
      @config[:profiles].should include("spec/profiles/rails.yaml")
    end
  end
end

describe "The -W, --excl-profile FILE option" do
  before :each do
    @options, @config = new_option
    @options.add_filters
  end

  it "is enabled with #add_filters" do
    @options.stub!(:on)
    @options.should_receive(:on).with("-W", "--excl-profile FILE",
        String, an_instance_of(String))
    @options.add_filters
  end

  it "adds FILE to the excluded profiles list" do
    ["-W", "--excl-profile"].each do |opt|
      @config[:xprofiles] = []
      @options.parse [opt, "spec/profiles/rails.yaml"]
      @config[:xprofiles].should include("spec/profiles/rails.yaml")
    end
  end
end

describe "The -Z", "--dry-run option" do
  before :each do
    @options, @config = new_option
    @options.add_pretend
  end

  it "is enabled with #add_pretend" do
    @options.should_receive(:on).with("-Z", "--dry-run", an_instance_of(String))
    @options.add_pretend
  end

  it "registers the MSpec pretend mode" do
    MSpec.should_receive(:register_mode).with(:pretend).twice
    ["-Z", "--dry-run"].each do |opt|
      @options.parse opt
    end
  end
end

describe "The -H, --random option" do
  before :each do
    @options, @config = new_option
    @options.add_randomize
  end

  it "is enabled with #add_randomize" do
    @options.should_receive(:on).with("-H", "--random", an_instance_of(String))
    @options.add_randomize
  end

  it "registers the MSpec randomize mode" do
    MSpec.should_receive(:randomize).twice
    ["-H", "--random"].each do |opt|
      @options.parse opt
    end
  end
end

describe "The -V, --verbose option" do
  before :each do
    @options, @config = new_option
    @options.add_verbose
  end

  it "is enabled with #add_verbose" do
    @options.stub!(:on)
    @options.should_receive(:on).with("-V", "--verbose", an_instance_of(String))
    @options.add_verbose
  end

  it "registers a verbose output object with MSpec" do
    MSpec.should_receive(:register).with(:start, anything()).twice
    MSpec.should_receive(:register).with(:load, anything()).twice
    ["-V", "--verbose"].each do |opt|
      @options.parse opt
    end
  end
end

describe "The -m, --marker MARKER option" do
  before :each do
    @options, @config = new_option
    @options.add_verbose
  end

  it "is enabled with #add_verbose" do
    @options.stub!(:on)
    @options.should_receive(:on).with("-m", "--marker MARKER",
        String, an_instance_of(String))
    @options.add_verbose
  end

  it "registers a marker output object with MSpec" do
    MSpec.should_receive(:register).with(:load, anything()).twice
    ["-m", "--marker"].each do |opt|
      @options.parse [opt, ","]
    end
  end
end

describe "The --int-spec option" do
  before :each do
    @options, @config = new_option
    @options.add_interrupt
  end

  it "is enabled with #add_interrupt" do
    @options.should_receive(:on).with("--int-spec", an_instance_of(String))
    @options.add_interrupt
  end

  it "sets the abort config option to false to only abort the running spec with ^C" do
    @config[:abort] = true
    @options.parse "--int-spec"
    @config[:abort].should == false
  end
end

describe "The -Y, --verify option" do
  before :each do
    @options, @config = new_option
    @options.add_verify
  end

  it "is enabled with #add_interrupt" do
    @options.stub!(:on)
    @options.should_receive(:on).with("-Y", "--verify", an_instance_of(String))
    @options.add_verify
  end

  it "sets the MSpec mode to :verify" do
    MSpec.should_receive(:set_mode).with(:verify).twice
    ["-Y", "--verify"].each do |m|
      @options.parse m
    end
  end
end

describe "The -O, --report option" do
  before :each do
    @options, @config = new_option
    @options.add_verify
  end

  it "is enabled with #add_interrupt" do
    @options.stub!(:on)
    @options.should_receive(:on).with("-O", "--report", an_instance_of(String))
    @options.add_verify
  end

  it "sets the MSpec mode to :report" do
    MSpec.should_receive(:set_mode).with(:report).twice
    ["-O", "--report"].each do |m|
      @options.parse m
    end
  end
end

describe "The -N, --add TAG option" do
  before :each do
    @options, @config = new_option
    @options.add_tagging
  end

  it "is enabled with #add_tagging" do
    @options.stub!(:on)
    @options.should_receive(:on).with("-N", "--add TAG",
        String, an_instance_of(String))
    @options.add_tagging
  end

  it "sets the mode to :add and sets the tag to TAG" do
    ["-N", "--add"].each do |opt|
      @config[:tagger] = nil
      @config[:tag] = nil
      @options.parse [opt, "taggit"]
      @config[:tagger].should == :add
      @config[:tag].should == "taggit:"
    end
  end
end

describe "The -R, --del TAG option" do
  before :each do
    @options, @config = new_option
    @options.add_tagging
  end

  it "is enabled with #add_tagging" do
    @options.stub!(:on)
    @options.should_receive(:on).with("-R", "--del TAG",
        String, an_instance_of(String))
    @options.add_tagging
  end

  it "it sets the mode to :del, the tag to TAG, and the outcome to :pass" do
    ["-R", "--del"].each do |opt|
      @config[:tagger] = nil
      @config[:tag] = nil
      @config[:outcome] = nil
      @options.parse [opt, "taggit"]
      @config[:tagger].should == :del
      @config[:tag].should == "taggit:"
      @config[:outcome].should == :pass
    end
  end
end

describe "The -Q, --pass option" do
  before :each do
    @options, @config = new_option
    @options.add_tagging
  end

  it "is enabled with #add_tagging" do
    @options.stub!(:on)
    @options.should_receive(:on).with("-Q", "--pass", an_instance_of(String))
    @options.add_tagging
  end

  it "sets the outcome to :pass" do
    ["-Q", "--pass"].each do |opt|
      @config[:outcome] = nil
      @options.parse opt
      @config[:outcome].should == :pass
    end
  end
end

describe "The -F, --fail option" do
  before :each do
    @options, @config = new_option
    @options.add_tagging
  end

  it "is enabled with #add_tagging" do
    @options.stub!(:on)
    @options.should_receive(:on).with("-F", "--fail", an_instance_of(String))
    @options.add_tagging
  end

  it "sets the outcome to :fail" do
    ["-F", "--fail"].each do |opt|
      @config[:outcome] = nil
      @options.parse opt
      @config[:outcome].should == :fail
    end
  end
end

describe "The -L, --all option" do
  before :each do
    @options, @config = new_option
    @options.add_tagging
  end

  it "is enabled with #add_tagging" do
    @options.stub!(:on)
    @options.should_receive(:on).with("-L", "--all", an_instance_of(String))
    @options.add_tagging
  end

  it "sets the outcome to :all" do
    ["-L", "--all"].each do |opt|
      @config[:outcome] = nil
      @options.parse opt
      @config[:outcome].should == :all
    end
  end
end

describe "The -K, --action-tag TAG option" do
  before :each do
    @options, @config = new_option
    @options.add_action_filters
  end

  it "is enabled with #add_action_filters" do
    @options.stub!(:on)
    @options.should_receive(:on).with("-K", "--action-tag TAG",
        String, an_instance_of(String))
    @options.add_action_filters
  end

  it "adds TAG to the list of tags that trigger actions" do
    ["-K", "--action-tag"].each do |opt|
      @config[:atags] = []
      @options.parse [opt, "action-tag"]
      @config[:atags].should include("action-tag")
    end
  end
end

describe "The -S, --action-string STR option" do
  before :each do
    @options, @config = new_option
    @options.add_action_filters
  end

  it "is enabled with #add_action_filters" do
    @options.stub!(:on)
    @options.should_receive(:on).with("-S", "--action-string STR",
        String, an_instance_of(String))
    @options.add_action_filters
  end

  it "adds STR to the list of spec descriptions that trigger actions" do
    ["-S", "--action-string"].each do |opt|
      @config[:astrings] = []
      @options.parse [opt, "action-str"]
      @config[:astrings].should include("action-str")
    end
  end
end

describe "The --spec-debug option" do
  before :each do
    @options, @config = new_option
    @options.add_actions
  end

  it "is enabled with #add_actions" do
    @options.stub!(:on)
    @options.should_receive(:on).with("--spec-debug", an_instance_of(String))
    @options.add_actions
  end

  it "enables the triggering the ruby debugger" do
    @options.add_action_filters
    @options.parse ["-S", "some spec"]

    @config[:debugger] = nil
    @options.parse "--spec-debug"
    @config[:debugger].should == true
  end
end

describe "The --spec-gdb option" do
  before :each do
    @options, @config = new_option
    @options.add_actions
  end

  it "is enabled with #add_actions" do
    @options.stub!(:on)
    @options.should_receive(:on).with("--spec-gdb", an_instance_of(String))
    @options.add_actions
  end

  it "enables triggering the gdb debugger" do
    @options.add_action_filters
    @options.parse ["-S", "some spec"]

    @config[:gdb] = nil
    @options.parse "--spec-gdb"
    @config[:gdb].should == true
  end
end

describe "The -v, --version option" do
  before :each do
    @options, @config = new_option
    @options.add_version
  end

  it "is enabled with #add_version" do
    @options.stub!(:on)
    @options.should_receive(:on).with("-v", "--version", an_instance_of(String))
    @options.add_version
  end

  it "prints the version and exits" do
    @options.should_receive(:puts).twice
    @options.should_receive(:exit).twice
    ["-v", "--version"].each do |opt|
      @options.parse opt
    end
  end
end

describe "The -h, --help option" do
  before :each do
    @options, @config = new_option
    @options.add_help
  end

  it "is enabled with #add_help" do
    @options.stub!(:on)
    @options.should_receive(:on).with("-h", "--help", an_instance_of(String))
    @options.add_help
  end

  it "prints help and exits" do
    @options.should_receive(:puts).twice
    @options.should_receive(:exit).twice
    ["-h", "--help"].each do |opt|
      @options.parse opt
    end
  end
end

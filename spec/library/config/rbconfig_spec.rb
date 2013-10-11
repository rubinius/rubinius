require File.expand_path('../../../spec_helper', __FILE__)
require 'rbconfig'

# These specs are difficult because the values may vary
# depending on platform or other configuration options.
# However, the specs are valuable in that they will ensure
# that rbconfig loads and contains required values.
#
# The RbConfig entries fall into two general categories:
# 1) the static-value entries; and 2) the configured-
# value entries. For the second one, the specs check
# only for the existence of a key.

def it_has_keys(name, keys)
  describe "#{name}" do
    before :all do
      @const = const_lookup name
    end

    keys.each do |key|
      it "has key #{key}" do
        @const.key?(key).should_not be_nil
      end
    end
  end
end

def it_has_entries(name, entries)
  describe "#{name}" do
    before :all do
      @const = const_lookup name
    end

    entries.each do |key, expected|
      it "has entry #{key} == #{expected}" do
        @const[key].should == expected
      end
    end
  end
end

describe "Config" do
  it "is equal to RbConfig" do
    Config.should eql(RbConfig)
  end
end

# TODO: many more specs needed
describe "RbConfig::CONFIG" do
  keys = [
    "LDSHARED",
    "DLEXT",
    "EXEEXT",
    "wordsize",
    "prefix",
    "bindir",
    "datadir",
    "rubyhdrdir",
    "wordsize",
    "arch",
    "bindir"
  ]

  it_has_keys 'RbConfig::CONFIG', keys

  entries = {
    "RUBY_SO_NAME"      => "rubinius-#{Rubinius::VERSION}",
    "ruby_install_name" => "rbx",
    "rubyhdrdir"        => Rubinius::HDR_PATH
  }

  it_has_entries 'RbConfig::CONFIG', entries
end

describe "RbConfig::MAKEFILE_CONFIG" do
  sitedir    = Rubinius::SITE_PATH
  sitelibdir = Rubinius::SITE_PATH
  arch       = "#{Rubinius::CPU}-#{Rubinius::OS}"

  entries = {
    "exec_prefix"        => "$(prefix)",
    "sbindir"            => "$(exec_prefix)/sbin",
    "libexecdir"         => "$(exec_prefix)/libexec",
    "datarootdir"        => "$(prefix)/share",
    "datadir"            => "$(datarootdir)",
    "sysconfdir"         => "$(prefix)/etc",
    "sharedstatedir"     => "$(prefix)/com",
    "localstatedir"      => "$(prefix)/var",
    "includedir"         => "$(prefix)/include",
    "oldincludedir"      => "/usr/include",
    "docdir"             => "$(datarootdir)/doc/$(PACKAGE)",
    "infodir"            => "$(datarootdir)/info",
    "htmldir"            => "$(docdir)",
    "dvidir"             => "$(docdir)",
    "pdfdir"             => "$(docdir)",
    "psdir"              => "$(docdir)",
    "libdir"             => "$(exec_prefix)/lib",
    "localedir"          => "$(datarootdir)/locale",
    "mandir"             => "$(datarootdir)/man",
    "ruby_version"       => "$(MAJOR).$(MINOR)",
    "rubylibdir"         => sitelibdir,
    "archdir"            => "#{sitelibdir}/#{arch}",
    "sitearchdir"        => "#{sitelibdir}/#{arch}",
    "sitedir"            => "#{sitedir}",
  }

  it_has_entries 'RbConfig::MAKEFILE_CONFIG', entries
end

describe "RbConfig#ruby" do

  ruby_version_is "1.8" do
    before :each do
      @version_switch = "-X18"
    end
  end

  ruby_version_is "1.9" do
    before :each do
      @version_switch = "-X19"
    end
  end

  ruby_version_is "2.0" do
    before :each do
      @version_switch = "-X20"
    end
  end

  it "returns the path to the running Ruby executable" do
    rb_path = RbConfig.ruby
    rb_path.should be_kind_of(String)

    # needs to be an executable
    File.executable?(rb_path).should eql(true)

    # make sure the executable it points to has the same RbConfig we have
    cmd = "#{rb_path} #{@version_switch} -rrbconfig -e 'puts Marshal.dump(RbConfig::CONFIG)'"
    rb_config = Marshal.load(`#{cmd}`)

    rb_config.should == RbConfig::CONFIG
  end
end

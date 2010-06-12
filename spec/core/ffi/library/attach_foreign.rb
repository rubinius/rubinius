require File.expand_path('../../spec_helper', __FILE__)

# TODO: This method no longer exists. Rework appropriate specs for
# #attach_function and remove this file

module FFISpecs
  LIB_EXTS   = [Rubinius::LIBSUFFIX]
  LIB_EXTS  << Rubinius::ALT_LIBSUFFIX rescue nil

  # To deterministically verify behaviour, we use relatively common
  # libraries and the likely paths in which they would reside. This
  # allows checking that an external library can be loaded and its
  # symbols become accessible.
  #
  # If your system has none of these libraries or they reside in a
  # different location, add that information here.
  #
  # TODO: Add more paths/libraries.

  KNOWN_LIBS = [{:name    => 'libcurl',
                 :symbol  => 'curl_version',
                 :args    => [],
                 :ret     => :string,
                 :check   => lambda {|ret| ret.kind_of? String and ret =~ /curl/ }
                }
               ]

  LIB_PATHS = %w[/usr/lib /usr/local/lib /opt/lib /opt/local/lib]
  LIB_NAMES = KNOWN_LIBS.map {|l| l[:name] }

  # Find a library that we can reliably check against
  def self.known_lib()
    @lib ||=  KNOWN_LIBS.find {|lib|
                LIB_PATHS.find {|path|
                  LIB_EXTS.find {|ext|
                    candidate = "#{path}/#{lib[:name]}.#{ext}"
                    if File.file? candidate
                      lib[:path] = candidate
                      lib[:full_name] = "#{lib[:name]}.#{ext}"
                      true
                    end
                  }
                }
              }
  end
end

describe "Function signature parsing in Module#attach_foreign" do
  # TBD
end

describe "Module#attach_foreign" do
  before :each do
    @mod = Module.new
  end

  it "raises FFI::SignatureError if function signature is given with more than 6 arguments" do
    lambda { @mod.attach_foreign(:int, 'foo', ([:int] * 7))}.should raise_error(FFI::SignatureError)
  end

  it "raises FFI::TypeError if any of the argument types is invalid (not in FFI::TypeDefs)" do
    lambda { @mod.attach_foreign :int, 'foo', [:bar] }.should raise_error(FFI::TypeError)
  end

  it "raises FFI::TypeError if the return type is invalid (not in FFI::TypeDefs)" do
    lambda { @mod.attach_foreign :foo, 'foo', [:int] }.should raise_error(FFI::TypeError)
  end

  it "accepts a hash of additional options as its fourth argument" do
    lambda { @mod.attach_foreign :string, 'ffi_tzname', [:int] }.should_not raise_error
    lambda { @mod.attach_foreign :string, 'ffi_tzname', [:int], {} }.should_not raise_error
    lambda { @mod.attach_foreign :string, 'ffi_tzname', [:int], :foo => :bar }.should_not raise_error
  end

  it "creates class method named as the function if function found in current process" do
    @mod.attach_foreign :string, 'ffi_tzname', [:int]
    @mod.methods.include?('ffi_tzname').should == true
  end

  it "accepts function name given as a Symbol" do
    @mod.attach_foreign :string, :ffi_tzname, [:int]
    @mod.methods.include?('ffi_tzname').should == true
  end

  it "accepts function name given as a String" do
    @mod.attach_foreign :string, 'ffi_tzname', [:int]
    @mod.methods.include?('ffi_tzname').should == true
  end

  it "creates the method for functions in current process that were linked in, not part of Rubinius' codebase" do
    @mod.attach_foreign :int, 'atoi', [:string]
    @mod.methods.include?('atoi').should == true
  end

  it "uses :as key of fourth argument as the method name if present" do
    @mod.attach_foreign :string, 'ffi_tzname', [:int], :as => 'my_tzname'

    @mod.methods.include?('ffi_tzname').should == false
    @mod.methods.include?('my_tzname').should == true
  end

  it "raises FFI::NotFoundError if function is not found in current process when no library name given" do
    lambda { @mod.attach_foreign :string, '__nOnEs_uChF_unC___', [] }.should raise_error(FFI::NotFoundError)
  end

  it "overwrites an existing method with same name" do
    def @mod.ffi_tzname; 'old'; end
    old = @mod.metaclass.method_table[:ffi_tzname]

    @mod.attach_foreign :string, 'ffi_tzname', [:int]

    @mod.metaclass.method_table[:ffi_tzname].eql?(old).should == false
  end

  it "overwrites an existing method with same name when name given in :as key of fourth argument" do
    def @mod.my_tzname; 'old'; end
    old = @mod.metaclass.method_table[:my_tzname]

    @mod.attach_foreign :string, 'ffi_tzname', [:int], :as => 'my_tzname'

    @mod.metaclass.method_table[:my_tzname].eql?(old).should == false
  end

  it "allows attaching the same function multiple times to different names" do
    @mod.attach_foreign :string, 'ffi_tzname', [:int]
    @mod.attach_foreign :string, 'ffi_tzname', [:int], :as => 'my_tzname'
    @mod.attach_foreign :string, 'ffi_tzname', [:int], :as => 'my_other_tzname'


    @mod.methods.include?('ffi_tzname').should == true
    @mod.methods.include?('my_tzname').should == true
    @mod.methods.include?('my_other_tzname').should == true
  end

  it "creates the method even if the return type given is incorrect for the function" do
    # The real declaration is
    #
    #   char* ffi_tzname(int dst);
    @mod.attach_foreign :double, 'ffi_tzname', [:int]
    @mod.methods.include?('ffi_tzname').should == true
  end

  it "creates the method even if any of the argument types given are incorrect for the function" do
    # The real declaration is
    #
    #   char* ffi_tzname(int dst);
    @mod.attach_foreign :string, 'ffi_tzname', [:string]
    @mod.methods.include?('ffi_tzname').should == true
  end
end


# Attempt to verify correct behaviour of external library access
# through some known, common libraries. If needed, add more libs
# at the top of the file.
describe "Module#attach_foreign additionally, when given library name in :from key of fourth parameter" do

  # Ungraceful but there are no better solutions right now
  raise "No known libraries found to verify FFI behaviour.\nPlease see #{__FILE__} to fix." unless FFISpecs.known_lib

  before :each do
    @mod = Module.new
    @known = FFISpecs.known_lib
  end

  it "creates method for function found in library named in the :from key of option hash" do
    @mod.attach_foreign @known[:ret], @known[:symbol], @known[:args], :from => @known[:name]
    @mod.methods.include?(@known[:symbol].to_s).should == true
  end

  it "accepts library name without a file extension" do
    @mod.attach_foreign @known[:ret], @known[:symbol], @known[:args], :from => @known[:name]
    @mod.methods.include?(@known[:symbol].to_s).should == true
  end

  it "accepts library name with valid shared lib extension included" do
    @mod.attach_foreign @known[:ret], @known[:symbol], @known[:args],
                        :from => @known[:full_name]
    @mod.methods.include?(@known[:symbol].to_s).should == true
  end

  it "does not strip a non-shared lib file extension" do
    l = lambda { @mod.attach_foreign @known[:ret], @known[:symbol], @known[:args], :from => "#{@known[:name]}.ext" }
    ll = lambda { @mod.attach_foreign @known[:ret], @known[:symbol], @known[:args], :from => "#{@known[:full_name]}.ext" }
    l.should raise_error(FFI::NotFoundError)
    ll.should raise_error(FFI::NotFoundError)
  end

  it "accepts library name given as full path" do
    @mod.attach_foreign @known[:ret], @known[:symbol], @known[:args], :from => @known[:path]
    @mod.methods.include?(@known[:symbol].to_s).should == true
  end

  it "raises FFI::NotFoundError if function does exist in current process but not in the library given" do
    l = lambda { @mod.attach_foreign :object, 'ffi_function_create', [:state, :object, :object, :object, :object], :from => @known[:name] }
    l.should raise_error(FFI::NotFoundError)
  end

  it "raises FFI::NotFoundError if function is not found in named library nor exists in the current process" do
    l = lambda { @mod.attach_foreign :string, 'rbx__nOnEs_uChF_unC___', [], :from => @known[:name] }
    l.should raise_error(FFI::NotFoundError)
  end
end


describe "Method created by attaching to a library function" do
  before :each do
    @mod = Module.new
    @mod.attach_foreign :string, 'ffi_tzname', [:int]
  end

  it "returns a Ruby object representation of the function's return value when called" do
    obj = @mod.ffi_tzname 0
    obj.class.should == String
    obj.length.should == 3
  end

  it "returns Ruby object of the return value of a function that was linked to Rubinius' codebase at buildtime" do
    @mod.attach_foreign :int, 'atoi', [:string]
    @mod.atoi('345').should == 345
  end

  it "raises TypeError if given an argument whose type does not correspond to type given in signature" do
    lambda { obj = @mod.ffi_tzname 'hello' }.should raise_error(TypeError)
  end
end

# Not sure what the correct behaviour should be anyway.
# These actually do not horribly crash (except for the
# last one.)
quarantine! do
  describe "Method created by attaching to a library function but with an incorrect return value type" do
    before :each do
      @mod = Module.new

      # The real declaration is
      #
      #   char* ffi_tzname(int dst);
      @mod.attach_foreign :double, 'ffi_tzname', [:int]
    end

    it "raises error when called and does not crash interpreter" do
      fail
    end
  end

  describe "Method created by attaching to a library function but with incorrect argument type(s)" do
    before :each do
      @mod = Module.new
      @mod.attach_foreign :string, 'ffi_tzname', [:string]
    end

    it "raises error when called with given argument type and does not crash interpreter" do
      fail
    end

    it "raises error when called with the correct not-given argument type and does not crash interpreter" do
      fail
    end
  end
end

describe :kernel_require_basic, :shared => true do
  describe "(path resolution)" do
    it "loads an absolute path" do
      path = File.expand_path "load_fixture.rb", CODE_LOADING_DIR
      @object.send(@method, path).should be_true
      ScratchPad.recorded.should == [:loaded]
    end

    it "loads a non-canonical absolute path" do
      dir, file = File.split(File.expand_path("load_fixture.rb", CODE_LOADING_DIR))
      path = File.join dir, ["..", "code"], file
      @object.send(@method, path).should be_true
      ScratchPad.recorded.should == [:loaded]
    end

    it "loads a file defining many methods" do
      path = File.expand_path "methods_fixture.rb", CODE_LOADING_DIR
      @object.send(@method, path).should be_true
      ScratchPad.recorded.should == [:loaded]
    end

    it "raises a LoadError if the file does not exist" do
      path = File.expand_path "nonexistent.rb", CODE_LOADING_DIR
      File.exists?(path).should be_false
      lambda { @object.send(@method, path) }.should raise_error(LoadError)
      ScratchPad.recorded.should == []
    end

    # Can't make a file unreadable on these platforms
    platform_is_not :os => [:windows, :cygwin] do
      describe "with an unreadable file" do
        before :each do
          @path = tmp("unreadable_file.rb")
          touch @path
          File.chmod 0000, @path
        end

        after :each do
          File.chmod 0666, @path
          rm_r @path
        end

        it "raises a LoadError" do
          File.exists?(@path).should be_true
          lambda { @object.send(@method, @path) }.should raise_error(LoadError)
        end
      end
    end

    it "calls #to_str on non-String objects" do
      path = File.expand_path "load_fixture.rb", CODE_LOADING_DIR
      name = mock("load_fixture.rb mock")
      name.should_receive(:to_str).and_return(path)
      @object.send(@method, name).should be_true
      ScratchPad.recorded.should == [:loaded]
    end

    it "raises a TypeError if passed nil" do
      lambda { @object.send(@method, nil) }.should raise_error(TypeError)
    end

    it "raises a TypeError if passed a Fixnum" do
      lambda { @object.send(@method, 42) }.should raise_error(TypeError)
    end

    it "raises a TypeError if passed an Array" do
      lambda { @object.send(@method, []) }.should raise_error(TypeError)
    end

    it "raises a TypeError if passed an object that does not provide #to_str" do
      lambda { @object.send(@method, mock("not a filename")) }.should raise_error(TypeError)
    end

    it "raises a TypeError if passed an object that has #to_s but not #to_str" do
      name = mock("load_fixture.rb mock")
      name.stub!(:to_s).and_return("load_fixture.rb")
      $LOAD_PATH << "."
      Dir.chdir CODE_LOADING_DIR do
        lambda { @object.send(@method, name) }.should raise_error(TypeError)
      end
    end

    it "raises a TypeError if #to_str does not return a String" do
      name = mock("#to_str returns nil")
      name.should_receive(:to_str).at_least(1).times.and_return(nil)
      lambda { @object.send(@method, name) }.should raise_error(TypeError)
    end

    ruby_version_is "".."1.9" do
      it "raises a TypeError if passed an object that has #to_path but not #to_str" do
        name = mock("load_fixture.rb mock")
        name.stub!(:to_path).and_return("load_fixture.rb")
        $LOAD_PATH << "."
        Dir.chdir CODE_LOADING_DIR do
          lambda { @object.send(@method, name) }.should raise_error(TypeError)
        end
      end
    end

    ruby_version_is "1.9" do
      it "calls #to_path on non-String objects" do
        name = mock("load_fixture.rb mock")
        name.stub!(:to_path).and_return("load_fixture.rb")
        $LOAD_PATH << "."
        Dir.chdir CODE_LOADING_DIR do
          @object.send(@method, name).should be_true
        end
        ScratchPad.recorded.should == [:loaded]
      end

      it "calls #to_path on a String" do
        path = File.expand_path "load_fixture.rb", CODE_LOADING_DIR
        str = mock("load_fixture.rb mock")
        str.should_receive(:to_path).and_return(path)
        @object.send(@method, str).should be_true
        ScratchPad.recorded.should == [:loaded]
      end

      it "calls #to_str on non-String objects returned by #to_path" do
        path = File.expand_path "load_fixture.rb", CODE_LOADING_DIR
        name = mock("load_fixture.rb mock")
        to_path = mock("load_fixture_rb #to_path mock")
        name.should_receive(:to_path).and_return(to_path)
        to_path.should_receive(:to_str).and_return(path)
        @object.send(@method, name).should be_true
        ScratchPad.recorded.should == [:loaded]
      end
    end

    ruby_bug "http://redmine.ruby-lang.org/issues/show/2578", "1.8" do
      it "loads a ./ relative path from the current working directory with empty $LOAD_PATH" do
        Dir.chdir CODE_LOADING_DIR do
          @object.send(@method, "./load_fixture.rb").should be_true
        end
        ScratchPad.recorded.should == [:loaded]
      end

      it "loads a ../ relative path from the current working directory with empty $LOAD_PATH" do
        Dir.chdir CODE_LOADING_DIR do
          @object.send(@method, "../code/load_fixture.rb").should be_true
        end
        ScratchPad.recorded.should == [:loaded]
      end
    end

    it "loads a ./ relative path from the current working directory with non-empty $LOAD_PATH" do
      $LOAD_PATH << "an_irrelevant_dir"
      Dir.chdir CODE_LOADING_DIR do
        @object.send(@method, "./load_fixture.rb").should be_true
      end
      ScratchPad.recorded.should == [:loaded]
    end

    it "loads a ../ relative path from the current working directory with non-empty $LOAD_PATH" do
      $LOAD_PATH << "an_irrelevant_dir"
      Dir.chdir CODE_LOADING_DIR do
        @object.send(@method, "../code/load_fixture.rb").should be_true
      end
      ScratchPad.recorded.should == [:loaded]
    end

    it "loads a non-canonical path from the current working directory with non-empty $LOAD_PATH" do
      $LOAD_PATH << "an_irrelevant_dir"
      Dir.chdir CODE_LOADING_DIR do
        @object.send(@method, "../code/../code/load_fixture.rb").should be_true
      end
      ScratchPad.recorded.should == [:loaded]
    end

    it "resolves a filename against $LOAD_PATH entries" do
      $LOAD_PATH << CODE_LOADING_DIR
      @object.send(@method, "load_fixture.rb").should be_true
      ScratchPad.recorded.should == [:loaded]
    end

    it "does not resolve a ./ relative path against $LOAD_PATH entries" do
      $LOAD_PATH << CODE_LOADING_DIR
      lambda do
        @object.send(@method, "./load_fixture.rb")
      end.should raise_error(LoadError)
      ScratchPad.recorded.should == []
    end

    it "does not resolve a ../ relative path against $LOAD_PATH entries" do
      $LOAD_PATH << CODE_LOADING_DIR
      lambda do
        @object.send(@method, "../code/load_fixture.rb")
      end.should raise_error(LoadError)
      ScratchPad.recorded.should == []
    end

    it "resolves a non-canonical path against $LOAD_PATH entries" do
      $LOAD_PATH << File.dirname(CODE_LOADING_DIR)
      @object.send(@method, "code/../code/load_fixture.rb").should be_true
      ScratchPad.recorded.should == [:loaded]
    end

    it "loads a path with duplicate path separators" do
      $LOAD_PATH << "."
      sep = File::Separator + File::Separator
      path = ["..", "code", "load_fixture.rb"].join(sep)
      Dir.chdir CODE_LOADING_DIR do
        @object.send(@method, path).should be_true
      end
      ScratchPad.recorded.should == [:loaded]
    end
  end
end

describe :kernel_require, :shared => true do
  describe "(path resolution)" do
    ruby_version_is "1.9" do
      # For reference see [ruby-core:24155] in which matz confirms this feature is
      # intentional for security reasons.
      it "does not load a bare filename unless the current working directory is in $LOAD_PATH" do
        Dir.chdir CODE_LOADING_DIR do
          lambda { @object.require("load_fixture.rb") }.should raise_error(LoadError)
          ScratchPad.recorded.should == []
        end
      end

      it "does not load a relative path unless the current working directory is in $LOAD_PATH" do
        Dir.chdir File.dirname(CODE_LOADING_DIR) do
          lambda do
            @object.require("code/load_fixture.rb")
          end.should raise_error(LoadError)
          ScratchPad.recorded.should == []
        end
      end
    end

    it "loads a file that recursively requires itself" do
      path = File.expand_path "recursive_require_fixture.rb", CODE_LOADING_DIR
      @object.require(path).should be_true
      ScratchPad.recorded.should == [:loaded]
    end
  end

  describe "(file extensions)" do
    it "loads a .rb extensioned file when passed a non-extensioned path" do
      path = File.expand_path "load_fixture", CODE_LOADING_DIR
      File.exists?(path).should be_true
      @object.require(path).should be_true
      ScratchPad.recorded.should == [:loaded]
    end

    it "loads a .rb extensioned file when a C-extension file of the same name is loaded" do
      $LOADED_FEATURES << File.expand_path("load_fixture.bundle", CODE_LOADING_DIR)
      $LOADED_FEATURES << File.expand_path("load_fixture.dylib", CODE_LOADING_DIR)
      $LOADED_FEATURES << File.expand_path("load_fixture.so", CODE_LOADING_DIR)
      $LOADED_FEATURES << File.expand_path("load_fixture.dll", CODE_LOADING_DIR)
      path = File.expand_path "load_fixture", CODE_LOADING_DIR
      @object.require(path).should be_true
      ScratchPad.recorded.should == [:loaded]
    end

    it "does not load a C-extension file if a .rb extensioned file is already loaded" do
      $LOADED_FEATURES << File.expand_path("load_fixture.rb", CODE_LOADING_DIR)
      path = File.expand_path "load_fixture", CODE_LOADING_DIR
      @object.require(path).should be_false
      ScratchPad.recorded.should == []
    end

    it "loads a .rb extensioned file when passed a non-.rb extensioned path" do
      path = File.expand_path "load_fixture.ext", CODE_LOADING_DIR
      File.exists?(path).should be_true
      @object.require(path).should be_true
      ScratchPad.recorded.should == [:loaded]
    end

    it "loads a .rb extensioned file when a complex-extensioned C-extension file of the same name is loaded" do
      $LOADED_FEATURES << File.expand_path("load_fixture.ext.bundle", CODE_LOADING_DIR)
      $LOADED_FEATURES << File.expand_path("load_fixture.ext.dylib", CODE_LOADING_DIR)
      $LOADED_FEATURES << File.expand_path("load_fixture.ext.so", CODE_LOADING_DIR)
      $LOADED_FEATURES << File.expand_path("load_fixture.ext.dll", CODE_LOADING_DIR)
      path = File.expand_path "load_fixture.ext", CODE_LOADING_DIR
      @object.require(path).should be_true
      ScratchPad.recorded.should == [:loaded]
    end

    it "does not load a C-extension file if a complex-extensioned .rb file is already loaded" do
      $LOADED_FEATURES << File.expand_path("load_fixture.ext.rb", CODE_LOADING_DIR)
      path = File.expand_path "load_fixture.ext", CODE_LOADING_DIR
      @object.require(path).should be_false
      ScratchPad.recorded.should == []
    end
  end

  describe "($LOAD_FEATURES)" do
    before :each do
      @path = File.expand_path("load_fixture.rb", CODE_LOADING_DIR)
    end

    it "stores an absolute path" do
      @object.require(@path).should be_true
      $LOADED_FEATURES.should == [@path]
    end

    it "does not store the path if the load fails" do
      $LOAD_PATH << CODE_LOADING_DIR
      lambda { @object.require("raise_fixture.rb") }.should raise_error(RuntimeError)
      $LOADED_FEATURES.should == []
    end

    it "does not load an absolute path that is already stored" do
      $LOADED_FEATURES << @path
      @object.require(@path).should be_false
      ScratchPad.recorded.should == []
    end

    it "does not load a ./ relative path that is already stored" do
      $LOADED_FEATURES << "./load_fixture.rb"
      Dir.chdir CODE_LOADING_DIR do
        @object.require("./load_fixture.rb").should be_false
      end
      ScratchPad.recorded.should == []
    end

    it "does not load a ../ relative path that is already stored" do
      $LOADED_FEATURES << "../load_fixture.rb"
      Dir.chdir CODE_LOADING_DIR do
        @object.require("../load_fixture.rb").should be_false
      end
      ScratchPad.recorded.should == []
    end

    it "does not load a non-canonical path that is already stored" do
      $LOADED_FEATURES << "code/../code/load_fixture.rb"
      $LOAD_PATH << File.dirname(CODE_LOADING_DIR)
      @object.require("code/../code/load_fixture.rb").should be_false
      ScratchPad.recorded.should == []
    end

    ruby_version_is "".."1.9" do
      it "stores ./ relative paths as passed in" do
        $LOAD_PATH << "an_irrelevant_dir"
        Dir.chdir CODE_LOADING_DIR do
          @object.require("./load_fixture.rb").should be_true
        end
        $LOADED_FEATURES.should == ["./load_fixture.rb"]
      end

      it "stores ../ relative paths as passed in" do
        $LOAD_PATH << "an_irrelevant_dir"
        Dir.chdir CODE_LOADING_DIR do
          @object.require("../code/load_fixture.rb").should be_true
        end
        $LOADED_FEATURES.should == ["../code/load_fixture.rb"]
      end

      it "does not collapse duplicate path separators" do
        $LOAD_PATH << "."
        sep = File::Separator + File::Separator
        path = ["..", "code", "load_fixture.rb"].join(sep)
        Dir.chdir CODE_LOADING_DIR do
          @object.require(path).should be_true
        end
        $LOADED_FEATURES.should == [path]
      end

      it "adds the suffix of the resolved filename" do
        $LOAD_PATH << CODE_LOADING_DIR
        @object.require("load_fixture").should be_true
        $LOADED_FEATURES.should == ["load_fixture.rb"]
      end

      it "loads a non-canonical path for a file already loaded" do
        $LOADED_FEATURES << @path
        $LOAD_PATH << File.dirname(CODE_LOADING_DIR)
        @object.require("code/../code/load_fixture.rb").should be_true
        ScratchPad.recorded.should == [:loaded]
      end

      it "loads a ./ relative path for a file already loaded" do
        $LOADED_FEATURES << @path
        $LOAD_PATH << "an_irrelevant_dir"
        Dir.chdir CODE_LOADING_DIR do
          @object.require("./load_fixture.rb").should be_true
        end
        ScratchPad.recorded.should == [:loaded]
      end

      it "loads a ../ relative path for a file already loaded" do
        $LOADED_FEATURES << @path
        $LOAD_PATH << "an_irrelevant_dir"
        Dir.chdir CODE_LOADING_DIR do
          @object.require("../code/load_fixture.rb").should be_true
        end
        ScratchPad.recorded.should == [:loaded]
      end
    end

    ruby_version_is "1.9" do
      it "stores ../ relative paths as absolute paths" do
        Dir.chdir CODE_LOADING_DIR do
          @object.require("../code/load_fixture.rb").should be_true
        end
        $LOADED_FEATURES.should == [@path]
      end

      it "stores ./ relative paths as absolute paths" do
        Dir.chdir CODE_LOADING_DIR do
          @object.require("./load_fixture.rb").should be_true
        end
        $LOADED_FEATURES.should == [@path]
      end

      it "collapses duplicate path separators" do
        $LOAD_PATH << "."
        sep = File::Separator + File::Separator
        path = ["..", "code", "load_fixture.rb"].join(sep)
        Dir.chdir CODE_LOADING_DIR do
          @object.require(path).should be_true
        end
        $LOADED_FEATURES.should == [@path]
      end

      it "canonicalizes non-unique absolute paths" do
        dir, file = File.split(File.expand_path("load_fixture.rb", CODE_LOADING_DIR))
        path = File.join dir, ["..", "code"], file
        @object.require(path).should be_true
        $LOADED_FEATURES.should == [@path]
      end

      it "adds the suffix of the resolved filename" do
        $LOAD_PATH << CODE_LOADING_DIR
        @object.require("load_fixture").should be_true
        $LOADED_FEATURES.should == [@path]
      end

      it "does not load a non-canonical path for a file already loaded" do
        $LOADED_FEATURES << @path
        $LOAD_PATH << File.dirname(CODE_LOADING_DIR)
        @object.require("code/../code/load_fixture.rb").should be_false
        ScratchPad.recorded.should == []
      end

      it "does not load a ./ relative path for a file already loaded" do
        $LOADED_FEATURES << @path
        $LOAD_PATH << "an_irrelevant_dir"
        Dir.chdir CODE_LOADING_DIR do
          @object.require("./load_fixture.rb").should be_false
        end
        ScratchPad.recorded.should == []
      end

      it "does not load a ../ relative path for a file already loaded" do
        $LOADED_FEATURES << @path
        $LOAD_PATH << "an_irrelevant_dir"
        Dir.chdir CODE_LOADING_DIR do
          @object.require("../code/load_fixture.rb").should be_false
        end
        ScratchPad.recorded.should == []
      end
    end
  end

  describe "(shell expansion)" do
    before :all do
      @env_home = ENV["HOME"]
      ENV["HOME"] = CODE_LOADING_DIR
    end

    after :all do
      ENV["HOME"] = @env_home
    end

    ruby_version_is "".."1.9" do
      it "does not perform tilde expansion before storing paths in $LOADED_FEATURES" do
        @object.require("~/load_fixture.rb").should be_true
        $LOADED_FEATURES.should == ["~/load_fixture.rb"]
      end
    end

    ruby_version_is "1.9" do
      it "performs tilde expansion before storing paths in $LOADED_FEATURES" do
        path = File.expand_path("load_fixture.rb", CODE_LOADING_DIR)
        @object.require("~/load_fixture.rb").should be_true
        $LOADED_FEATURES.should == [path]
      end
    end
  end
end

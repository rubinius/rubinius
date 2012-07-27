# -*- encoding: us-ascii -*-

module Rubinius
  class CodeLoader

    # Searches $LOAD_PATH for a file named +name+. Does not append any file
    # extension to +name+ while searching. Used by #load to resolve the name
    # to a full path to load. Also used by #require when the file extension is
    # provided.
    def search_load_path(name, loading)
      $LOAD_PATH.each do |dir|
        path = "#{dir}/#{name}"
        return path if loadable? path
      end

      return name if loading and loadable? "./#{name}"

      return nil
    end

    # requires files relative to the current directory. We do one interesting
    # check to make sure it's not called inside of an eval.
    def self.require_relative(name, scope)
      script = scope.current_script
      if script && script.data_path.nil?
        raise LoadError.new "cannot infer basepath from #{script.file_path}"
      elsif script
        require File.expand_path(name, File.dirname(script.data_path))
      else
        raise LoadError.new "Something is wrong in trying to get relative path"
      end
    end

    # Sets +@feature+, +@file_path+, +@load_path+ with the correct format.
    # Used by #verify_load_path, #check_path and #check_file.
    def update_paths(file, path)
      path = File.expand_path path

      @feature = path
      @file_path = path
      @load_path = path
    end

    # Main logic for converting a name to an actual file to load. Used by
    # #load and by #require when the file extension is provided.
    #
    # Expands any #home_path? to an absolute path. Then either checks whether
    # an absolute path is #loadable? or searches for a loadable file matching
    # name in $LOAD_PATH.
    #
    # Returns true if a loadable file is found, otherwise returns false.
    def verify_load_path(path, loading=false)
      path = File.expand_path path if home_path? path

      if qualified_path? path
        return false unless loadable? path
      else
        return false unless path = search_load_path(path, loading)
      end

      update_paths(path, path)

      return true
    end
  end
end

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
        require File.expand_path(name, File.realdirpath(script.data_path))
      else
        raise LoadError.new "Something is wrong in trying to get relative path"
      end
    end

    # Sets +@feature+, +@file_path+, +@load_path+ with the correct format.
    # Used by #verify_load_path, #check_path and #check_file.
    def update_paths(file, path)
      path = File.expand_path path

      @feature    = path
      @short_path = file
      @file_path  = path
      @load_path  = path
    end
  end
end

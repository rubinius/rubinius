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

      return nil
    end

    # Sets +@feature+, +@file_path+, +@load_path+ with the correct format.
    # Used by #verify_load_path, #check_path and #check_file.
    def update_paths(file, path)
      path = "./#{path}" unless qualified_path? path

      @feature = file
      @file_path = path
      @load_path = File.expand_path path
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
        return false unless full_path = search_load_path(path, loading)
        update_paths(path, full_path)
        return true
      end

      update_paths(path, path)

      return true
    end
  end
end

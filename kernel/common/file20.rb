# -*- encoding: us-ascii -*-

class File

  def self.expand_path_buffer(path)
    path.to_s.dup.clear
  end
  private_class_method :expand_path_buffer

end

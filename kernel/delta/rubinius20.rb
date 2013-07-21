# -*- encoding: us-ascii -*-

module Rubinius
  def self.privatized_method?(name)
    [:initialize,
     :initialize_copy, :initialize_dup, :initialize_clone,
     :respond_to_missing?].include? name
  end
  private_class_method :privatized_method?
end

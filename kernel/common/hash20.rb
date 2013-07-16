# -*- encoding: us-ascii -*-

class Hash
  def self.new_from_associate_array(associate_array)
    hash = new
    associate_array.each do |array|
      next unless array.respond_to? :to_ary
      array = array.to_ary
      unless (1..2).cover? array.size
        raise ArgumentError, "invalid number of elements (#{array.size} for 1..2)"
      end
      hash[array.at(0)] = array.at(1)
    end
    hash
  end
  private_class_method :new_from_associate_array

  def to_h
    instance_of?(Hash) ? self : Hash[self]
  end

  # Sets the default proc to be executed on each key lookup
  def default_proc=(prc)
    Rubinius.check_frozen
    unless prc.nil?
      prc = Rubinius::Type.coerce_to prc, Proc, :to_proc

      if prc.lambda? and prc.arity != 2
        raise TypeError, "default proc must have arity 2"
      end
    end

    @default = nil
    @default_proc = prc
  end
end

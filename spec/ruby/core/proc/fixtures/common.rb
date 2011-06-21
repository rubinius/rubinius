module ProcSpecs
  def self.new_proc_in_method
    Proc.new
  end

  def self.new_proc_from_amp(&block)
    block
  end

  class ProcSubclass < Proc
  end

  def self.new_proc_subclass_in_method
    ProcSubclass.new
  end

  class MyProc < Proc
  end

  class MyProc2 < Proc
    def initialize(a, b)
      @first = a
      @second = b
    end

    attr_reader :first, :second
  end
end

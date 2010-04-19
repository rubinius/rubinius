module CodeLoadingSpecs
  def self.rbc_fixture(name)
    rb = tmp(name)
    rbc = rb + "c"

    if block_given?
      touch(rb) { |f| yield rb, rbc, f }
    else
      touch(rb) { |f| f.puts "ScratchPad << :loaded" }
    end

    Rubinius::Compiler.compile rb
    return rb, rbc
  end
end

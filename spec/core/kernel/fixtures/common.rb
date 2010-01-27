module CodeLoadingSpecs
  def self.rbc_fixture(name)
    rb = tmp(name)
    rbc = rb + "c"

    if block_given?
      touch(rb) { |f| yield rb, rbc, f }
    else
      touch(rb) { |f| f.puts "ScratchPad << :loaded" }
    end

    Kernel.compile rb
    return rb, rbc
  end

  def self.rba_fixture(name, file)
    rba = tmp("rba_fixture.rba")

    ar = Ar.new rba
    st = File.stat file
    ar.replace name, st.mtime, st.uid, st.gid, st.mode, IO.read(file)

    return rba
  end
end

# Test for a JIT bug where the while inside the if generated a goto
# of a loop header and skipped setup up the exception handler
# properly.
#
# If this doesn't crash, the bug is fixed.
def bad
  i = 0
  begin
    if i < 10
      while i < 20
        i += 2
      end
    end
  rescue => ex
    raise "failure is an option"
  end
  i
end

1000000.times do
  bad
end

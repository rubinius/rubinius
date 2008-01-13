shared :file_pipe do |cmd, klass, name|
  describe "#{name || "#{klass}.#{cmd}"}" do
  end
end

shared :file_setuid do |cmd, klass, name|
  describe "#{name || "#{klass}.#{cmd}"}" do
  end
end

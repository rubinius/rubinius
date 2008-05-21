shared :file_setgid do |cmd, klass, name|
  describe "#{name || "#{klass}.#{cmd}"}" do
  end
end

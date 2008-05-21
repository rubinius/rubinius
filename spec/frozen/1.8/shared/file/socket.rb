shared :file_socket do |cmd, klass, name|
  describe "#{name || "#{klass}.#{cmd}"}" do
  end
end

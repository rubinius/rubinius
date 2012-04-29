describe :marshal_load, :shared => true do
  it "loads an AtomicReference" do
    data = "\x04\bU:\x1ERubinius::AtomicReferencei\x00"
    atomic_reference = Marshal.send(@method, data)
    atomic_reference.class.should == Rubinius::AtomicReference
    atomic_reference.value.should == 0
  end
end

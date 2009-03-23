namespace rubinius {

  class RefCount {
    int ref_count_;

  public:

    RefCount() : ref_count_(0) {}

    void ref() {
      ref_count_++;
    }

    bool deref() {
      return --ref_count_ <= 0;
    }
  };

}

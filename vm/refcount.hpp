namespace rubinius {

  class RefCount {
    int ref_count_;

  public:
    void ref() {
      ref_count_++;
    }

    bool deref() {
      return --ref_count_ <= 0;
    }
  };

}
